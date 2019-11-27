

use crate::datastore;
use crate::network;

use std::time;
use std::thread;
use std::sync::{Arc};
use std::sync::atomic::{AtomicBool, Ordering};
use crossbeam::crossbeam_channel::{ Receiver, Sender, unbounded};

extern crate serde;
use serde::{Deserialize, Serialize};

use chrono;

/// Connection information and data to use for sending
#[derive(Clone, Debug)]
pub struct NodeMessage {
    pub connection: String,
    pub data:       Vec<u8>
}

/// Communicate with a node thread
pub enum NodeCommand {
    StopThread,             // Stop the thread
    SendData(NodeMessage),  // Send data from the thread
    Broadcast(Vec<u8>),     // Send data to all cached nodes
    RequestRegistration((String, String)), // Send a registration request to a remote node so they can broadcast to us   
}

/// Means of communicating with a node thread
pub struct NodeThreadInterface {

    pub thread_handle:            Option<thread::JoinHandle<()>>,
    pub data_receiver:            Receiver<NodeMessage>,
    pub command_sender:           Sender<NodeCommand>, 
}

// An internal enumeration to filter messages from TCP
#[derive(Clone, Debug, Serialize, Deserialize)]
enum InternalNodeCommand {
    RegisterConnection(String),                     // Register the current connection with the given ID
    RequestKnown,                                   // Request for all known nodes
    ResponseKnown(Vec<datastore::NetworkObject>),   // A response from the known nodes request
    ApplicationData(Vec<u8>)                        // Data meant for the application
}

/// Start a node
pub fn start(ip: String, port: String, id: String) -> Result<(NodeThreadInterface), &'static str> {

    // Connection
    let local_connection = format!("{}:{}", ip, port);

    // Channels for sending and receiving data to/from the application and endpoints
    let (app_data_sender,  app_data_receiver):  (Sender<NodeMessage>, Receiver<NodeMessage>) = unbounded();
    let (command_sender,   command_receiver) :  (Sender<NodeCommand>, Receiver<NodeCommand>) = unbounded();

    // Channel for communicating with TCP thread
    let (tcp_thread_sender, tcp_thread_receiver) : (Sender<network::Transport>, Receiver<network::Transport>) = unbounded();

    // Thread interface that the application will use to interact with the thread
    let mut nti : NodeThreadInterface = NodeThreadInterface{
        thread_handle:  None,
        data_receiver:  app_data_receiver,
        command_sender: command_sender
    };

    //
    //      Start the node thread that will be handed back to the caller
    //
    nti.thread_handle = Some(thread::spawn(move || {

        debug!("Starting {} @ {}:{}.", id, ip, port);

        let node_cache_file = format!("node-{}.json", id);
    
        // Load cache if it exists, otherwise attempt to create a new one.
        let mut node_cache = match datastore::get_data_store(&node_cache_file) {
            Ok(cache) => {
                debug!("Cache {} was loaded", node_cache_file);
                cache
            },
            Err(e) => {
                debug!("Could not load {} due to : {}", node_cache_file, e);
                debug!("Creating new cache...");
    
                datastore::new_data_store(&node_cache_file)
            }
        };

        let running_flag = Arc::new(AtomicBool::new(true));
        let app_command_receiver = command_receiver.clone();

        /*

            Start TCP
        
        */
        let running_flag_tcp_arc = Arc::clone(&running_flag);
        let tcp_handler_thread =  match network::get_receiver_thread(
            network::TcpSettings {
                ttl:                    1,
                read_timeout:           time::Duration::from_secs(1),
                connection:             local_connection.to_string(),
                thread_pool_size:       10,
                maximum_tcp_size:       2_000_000,
                alive_flag:             Arc::clone(&running_flag_tcp_arc),
                completed_data_sender:  tcp_thread_sender.clone()
        }) {
            Some(handler) => {
                handler
            },
            None => {
                running_flag.store(false, Ordering::SeqCst);
                panic!("Unable to start tcp handler!");
            }
        };

        /*
        
            Handle TCP messages and Node Commands until shut down signal is received
        
        */
        let mut sync_timer = chrono::Utc::now();
        while running_flag.load(Ordering::SeqCst) {

            // ------------------------------------------------------------------
            //                      Handle network data 
            // ------------------------------------------------------------------

            let mut node_update_flag: bool = false;
            let received_data : Vec<network::Transport> = tcp_thread_receiver.try_iter().collect();
            for received in received_data {

                /*
                
                    Deserialize the data we got from the network transport, and handle it accordingly

                */
                let deserialized_network_data : InternalNodeCommand = match serde_json::from_slice(
                    received.data.as_ref()
                ){
                    Ok(network_data) => {
                        network_data
                    },
                    Err(e) => {
                        warn!("Node was unable to deserialize network data {}", e);
                        continue;
                    }
                };

                match deserialized_network_data {

                    /*
                        Remote asked that we register their information
                    */
                    InternalNodeCommand::RegisterConnection(id) => {

                        node_cache.insert(&id, received.connection_info);
                        node_update_flag = true;
                    },

                    /*
                        Remote asked for our nodes
                    */
                    InternalNodeCommand::RequestKnown => {
                        
                        // Get all network objects, and serialize them
                        match serde_json::to_vec(&InternalNodeCommand::ResponseKnown(node_cache.retrieve_all_network_objects())) {
                            Ok(serialized) => {
                                
                                // Send them
                                if let Err(e) = network::send(local_connection.clone(), received.connection_info, serialized) {
                                    warn!("Unable to send node list response {}", e);
                                }
                            },
                            Err(e) => {
                                warn!("Error serializing network objects for remote request: {}", e);
                                continue;
                            }
                        }
                    },

                    /*
                        Remote responded with known nodes
                    */
                    InternalNodeCommand::ResponseKnown(transport_list) => {

                        for transport_item in transport_list {

                            if transport_item.value != local_connection.clone() {
                                node_cache.insert(&transport_item.key, transport_item.value);
                            }
                        }

                        node_update_flag = true;
                    },

                    /*
                        Send the application some data
                    */
                    InternalNodeCommand::ApplicationData(data) => {

                        if let Err(e) = app_data_sender.send(NodeMessage{
                            connection: received.connection_info,
                            data:       data
                        }){
                            warn!("Got ApplicationData request, but was unable to send to application {}", e);
                        }
                    }
                }
            } // End TCP message handle

            /*
            
                If the node cache was updated, then write it out
            
            */
            if node_update_flag {

                if let Err(e) = node_cache.write() {
                    warn!("Unable to update node cache : {}", e);
                }
            }

            // ------------------------------------------------------------------
            //                      Handle inbound commands
            // ------------------------------------------------------------------

            let app_requests : Vec<NodeCommand> = app_command_receiver.try_iter().collect();

            for request in app_requests {

                match request {
                    NodeCommand::StopThread => {
                        info!("Node thread received command to shut down");
                        running_flag.store(false, Ordering::SeqCst);
                    },

                    /*
                        Send data from application directly to a remote node
                    */
                    NodeCommand::SendData(node_message) =>{

                        let send_enum : InternalNodeCommand = InternalNodeCommand::ApplicationData(node_message.data);

                        // Make Application data enum with node_message data, and send to network::send. 
                        match serde_json::to_vec(&send_enum) {
                            Ok(serialized) => {
                                
                                // Send them
                                if let Err(e) = network::send(local_connection.clone(), node_message.connection, serialized) {
                                    warn!("Unable to send node application data {}", e);
                                }
                            },
                            Err(e) => {
                                warn!("Error serializing network objects for remote request: {}", e);
                                continue;
                            }
                        }
                    }

                    /*
                        Broadcast application data to all remote nodes
                    */
                    NodeCommand::Broadcast(broadcast_data) => {

                        let network_objects = node_cache.retrieve_all_network_objects();

                        for obj in network_objects{

                            match serde_json::to_vec(&InternalNodeCommand::ApplicationData(broadcast_data.clone())) {
                                Ok(serialized) => {
                                    
                                    // Send them
                                    if let Err(e) = network::send(local_connection.clone(), obj.value, serialized) {
                                        warn!("Unable to broadcast node application data {}", e);
                                    }
                                },
                                Err(e) => {
                                    warn!("Error serializing network objects for remote request: {}", e);
                                    continue;
                                }
                            }
                        }
                    }

                    /*
                        Request a registration
                    */
                    NodeCommand::RequestRegistration((id, remote)) => {
                        match serde_json::to_vec(&InternalNodeCommand::RegisterConnection(id.clone())) {
                            Ok(serialized) => {
                                
                                // Send them
                                if let Err(e) = network::send(local_connection.clone(), remote, serialized) {
                                    warn!("Unable to broadcast node application data {}", e);
                                }
                            },
                            Err(e) => {
                                warn!("Error serializing network objects for remote request: {}", e);
                                continue;
                            }
                        }
                    }

                } // End node command request
            } // End Node command handler loop

            // ------------------------------------------------------------------
            //                      Handle periodic commands
            // ------------------------------------------------------------------

            if chrono::Utc::now() - sync_timer > chrono::Duration::seconds(30) {

                // Send a sync request to all peers

                let network_objects = node_cache.retrieve_all_network_objects();

                for obj in network_objects{

                    match serde_json::to_vec(&InternalNodeCommand::RequestKnown) {
                        Ok(serialized) => {
                            
                            // Send them
                            if let Err(e) = network::send(local_connection.clone(), obj.value, serialized) {
                                warn!("Unable to broadcast node application data {}", e);
                            }
                        },
                        Err(e) => {
                            warn!("Error serializing network objects for remote request: {}", e);
                            continue;
                        }
                    }
                }

                sync_timer = chrono::Utc::now();
            } // End sync reqyest

            // ------------------------------------------------------------------

            thread::sleep(time::Duration::from_micros(100));
        }
        // Join inner threads
        tcp_handler_thread.join().expect("Could not join tcp handler thread");

        // Save the node cache
        if let Err(e) = node_cache.write() {
            warn!("Unable to save node cache before exit : {}", e);
        }
    }));

    Ok(nti)
}