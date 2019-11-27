
use crate::system;

use chrono;
use std::cell::RefCell;
use std::sync::{ Arc, Mutex };
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;
use std::time;
use std::collections::HashMap;
use serde::{Deserialize, Serialize};
use crossbeam::crossbeam_channel::unbounded;
use crossbeam::crossbeam_channel::Receiver as CrossRecv;
use crossbeam::crossbeam_channel::Sender   as CrossSend;

/*
    Development notes:

    Since the application will be aggregating information from multiple networks (potentially) we will need to tie-together
    which network_manager the data came from so when the application user responds it goes to the right network so it can reach the 
    right peer

    Will need to check carbon_app during queries and message processing to see if the application should act as a bridge for incomming requiests

    -- Since the Dark Carbon library can sit on MULTIPLE nexus loacals, an id specific to the instance may need to be created

    Network manager methods available to the control panel: 
        app_broadcast_message           // Send data out to the entire network
        app_broadcast_registration      // Send registration requests out to entire network
        app_peer_synchronization        // Sync peers from the network 
        app_direct_send                 // Send directly to a peer id known to the manager
        app_remove_inactive             // Remove inactive nodes from the manager
        app_get_manager_id              // Get the id of the manager
        app_get_peers                   // Get peers known to the manager

    Network manager items available:
        app_data_receiver               // This is where we will get data from
*/

#[derive(Clone, Debug, Serialize, Deserialize)]
struct DcRouteInfo {
    application_origin:     String,     // Id of origin application
    network_destination:    String,     // Id of destination network
    node_destination:       String      // Id of destination node
}

#[derive(Clone, Debug, Serialize, Deserialize)]
enum SpecificQuery {
    NodeTags,                // Requesting the tags of the node
    NodeDescription,         // Requesting node description
    IsRelay,                 // Requesting node is a relay
    CanReachNode(String),    // Request to check if node is reachable (String == id of node in question)
    CanReachNetwork(String)  // Request to check if network is reachable (String == id of network in question)
}

#[derive(Clone, Debug, Serialize, Deserialize)]
struct DcQuery {
    route_info:   DcRouteInfo,      // Route information to to determine who the query is for
    query:        SpecificQuery     // Query data
}

//  Used to determine what the information in a data wrapper is meant for
#[derive(Clone, Debug, Serialize, Deserialize)]
enum DcDataDiscriminator {
    Relay(DcRouteInfo),                         // Relay Information from a node on one network to a node on another network
    Broadcast(String),                          // Broadcast message, contains origin
    PointToPoint,                               // Message from sender is intended for this application
    Query(DcQuery)                              // Query to dc node
}

//  A data wrapper that is used to push information through a nexus network
#[derive(Clone, Debug, Serialize, Deserialize)]
struct DcDataWrapper {

    discriminator:          DcDataDiscriminator, // Enum representing what the message is for
    raw_data:               Vec<u8>              // Application data
}

// Instructions / informaton for handling data within a send channel
#[derive(Debug)]
struct DcSendable {
    node_destination:    String,
    network_destination: String,
    wrapped_data:        DcDataWrapper,
    ttl:                 i32
}

//
/// A structure used to control the application once it is running
//
pub struct ControlPanel {

    app_running:           Arc<AtomicBool>, 
    network_managers:      Arc<Mutex<Vec<nexus::network::NetworkManager>>>,       // Network managers for each network that the DC Application is part of
    local_arcs:            RefCell<Vec<Arc<Mutex<nexus::system::Local>>>>,        // This list acts only as a memory placement
    identifiers:           Arc<Mutex<Vec<String>>>,                               // All identifiers that this node represents
    network_map:           Arc<Mutex<HashMap<String, Vec<nexus::system::Peer>>>>, // Map of network ids to peers on that network. 
    nexus_input_handler:   Option<thread::JoinHandle<()>>,                        // Thread handler
    nexus_update_handler:  Option<thread::JoinHandle<()>>,                        // Thread handler
    carbon_app:            Arc<Mutex<system::CarbonApp>>,                         // Application information
    pub app_data_receiver: CrossRecv<system::CarbonMsg>,                          // Data meant for the application will be received here here
    app_data_sender:       CrossSend<system::CarbonMsg>,                          // Data meant for the application will be sent here
    broadcast_outbound:    CrossRecv<DcDataWrapper>,                              // Data sent to be broadcasted will be grapped from here to be sent
    broadcast_sender:      CrossSend<DcDataWrapper>,                              // Input for broadcast data channel
    relay_outbound:        CrossRecv<DcSendable>,        
    relay_sender:          CrossSend<DcSendable>,
    direct_send_outbound:  CrossRecv<DcSendable>,
    direct_send_sender:    CrossSend<DcSendable>,
}

impl ControlPanel {

    /// Shutdown the application
    pub fn shutdown(self: &mut Self) {
        self.app_running.store(false, Ordering::SeqCst);

        self.nexus_input_handler
            .take().expect("Called stop on non-running thread [input handler]")
                .join().expect("Could not join server thread");

        self.nexus_update_handler
            .take().expect("Called stop on non-running thread [update handler]")
                .join().expect("Could not join server thread");
    }

    /// Start the application
    pub fn start(self: &mut Self) -> Result<(), &'static str> {


        if self.app_running.load(Ordering::SeqCst) {
            return Err("Application is already running");
        }

        self.app_running.store(true, Ordering::SeqCst);

        // Start the network managers and broadcast registration to the network
        let mut netman_guard = match self.network_managers.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        for manager in netman_guard.iter_mut() {

            if let Err(e) = manager.start() {
                return Err(e);
            }

            if let Err(e) = manager.app_broadcast_registration() {
                warn!("Failed to broadcast registration request to known peers : {}", e);
            }
        }

        drop(netman_guard);

        // Start data input manager
        if let Err(e) = self.begin_nexus_data_handler() {
            return Err(e);
        }

        // Start nexus updater
        if let Err(e) = self.begin_nexus_updater() {
            return Err(e);
        }

        Ok(())
    }

    /// Retrieve the network map <Network Id> -> [peer list]; 
    /// If the network map is being updated at the moment of the call, an error will be returned
    pub fn get_network_map(self: &Self) -> HashMap<String, Vec<nexus::system::Peer>> {

        let netmap_guard = match self.network_map.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        netmap_guard.clone()
    }

    /// Send some data to a destination. If destination can't be immediatly reached, a relay message will be send to all known nodes
    pub fn send(self: &Self, destination: system::MessageHeader, raw_data: Vec<u8>, ttl: i32) {

        warn!("direct_send called but has not yet been completed");

        let map_guard = match self.network_map.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let mut found = false;
        'map_find: for (key, _value) in map_guard.iter() {
            if key.to_string() == destination.network.to_string() {
                found = true;
                break 'map_find;
            }
        }
        drop(map_guard);

        if found {
            debug!("Constructing a direct send");

            if let Err(_) = self.direct_send_sender.send(
                DcSendable{
                    node_destination:    destination.network.clone(),
                    network_destination: destination.network.clone(),
                    wrapped_data:        DcDataWrapper{
                                            discriminator: DcDataDiscriminator::PointToPoint,
                                            raw_data:      raw_data.clone()
                                        },
                    ttl:                ttl
                }
            ){
                warn!("Unable to pipe relay message from control_panel.send");
                return;
            }

        } else {
            debug!("Constructing a relay send");

            let app_guard = match self.carbon_app.lock() {
                Ok(guard) => guard,
                Err(poisoned) => poisoned.into_inner(),
            };
    
            let local_app_id = app_guard.id.clone();
    
            drop(app_guard);

            if let Err(_) = self.relay_sender.send(
                DcSendable{
                    node_destination:    destination.peer_id.clone(),
                    network_destination: destination.network.clone(),
                    wrapped_data:        DcDataWrapper{
                                            discriminator: DcDataDiscriminator::Relay(
                                                DcRouteInfo{
                                                    application_origin:  local_app_id.clone(),
                                                    network_destination: destination.network.clone(),
                                                    node_destination:    destination.peer_id.clone()
                                                }),
                                                raw_data: raw_data.clone()
                                                },
                    ttl:                ttl
                }
            ){
                warn!("Unable to pipe relay message from control_panel.send");
                return;
            }
        }
    }

    /// Broadcast a message
    pub fn broadcast_send(self: &Self, raw_data: Vec<u8>) {

        let app_guard = match self.carbon_app.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let local_app_id = app_guard.id.clone();

        drop(app_guard);

        if let Err(_) = self.broadcast_sender.send(DcDataWrapper{

            discriminator: DcDataDiscriminator::Broadcast(local_app_id),
            raw_data: raw_data.clone()
        }){
            warn!("Error sending data to broadcast_sender channel");
        }
    }

    // Update known peers and remove offline peers every minute
    fn begin_nexus_updater(self: &mut Self) -> Result<(), &'static str> {

        let app_running_arc      = Arc::clone(&self.app_running);
        let network_manager_arc  = Arc::clone(&self.network_managers);
        let network_map_arc      = Arc::clone(&self.network_map);
        let thread_handler = Some(thread::spawn(move || {
            
            let mut last_sync = chrono::Utc::now();

            // While the application is indicated as running
            while app_running_arc.load(Ordering::SeqCst) {

                // Sync peers, and remove inactive nodes every minute
                if chrono::Utc::now().signed_duration_since(last_sync) > chrono::Duration::minutes(1) {

                    info!("Running nexus updater");
                    
                    let mut netman_guard = match network_manager_arc.lock() {
                        Ok(guard) => guard,
                        Err(poisoned) => poisoned.into_inner(),
                    };
    
                    let mut netmap_guard = match network_map_arc.lock() {
                        Ok(guard) => guard,
                        Err(poisoned) => poisoned.into_inner(),
                    };

                    // Go through the network managers
                    for manager in netman_guard.iter_mut() {

                        // Broadcast registration
                        if let Err(e) = manager.app_broadcast_registration() {
                            warn!("Failed to broadcast registration request to known peers : {}", e);
                        }

                        // Remove inactive nodes
                        manager.app_remove_inactive();
     
                        // Sync with active nodes
                        if let Err(e) = manager.app_peer_synchronization() {
                            info!("Failed to broadcast peer synchronization request to known peers : {}", e);
                        }

                        // Update the nework map
                        netmap_guard.insert(manager.app_get_manager_id(), manager.app_get_peers());
                    }
    
                    drop(netmap_guard);
                    drop(netman_guard);

                    last_sync = chrono::Utc::now();
                }

                // We sleep here and update last_sync for updating so the thread won't hang when user exits
                thread::sleep(time::Duration::from_millis(500));
            }
        }));

        self.nexus_update_handler = thread_handler;
        Ok(())
    }

    // Start the data input manager thread
    fn begin_nexus_data_handler(self: &mut Self) -> Result<(), &'static str> {

        let app_running_arc      = Arc::clone(&self.app_running);
        let network_manager_arc  = Arc::clone(&self.network_managers);
        let carbon_app_arc       = Arc::clone(&self.carbon_app);
        let identifiers_arc      = Arc::clone(&self.identifiers);
        let app_data_sender      = self.app_data_sender.clone();
        let broadcast_receiver   = self.broadcast_outbound.clone();
        let relay_reciever       = self.relay_outbound.clone();
        let direct_receiver      = self.direct_send_outbound.clone();

        let thread_handler = Some(thread::spawn(move || {
            
            // While the application is indicated as running
            while app_running_arc.load(Ordering::SeqCst) {

                let mut netman_guard = match network_manager_arc.lock() {
                    Ok(guard) => guard,
                    Err(poisoned) => poisoned.into_inner(),
                };

                // Go through the network managers
                for manager in netman_guard.iter_mut() {
 
                    // See if any data has been piped up from nexus
                    for app_data in manager.app_data_receiver.try_recv() {

                        // Attempt to deserialize app_data.raw_data into a DcDataWrapper

                        let wrapped_data : DcDataWrapper = match serde_json::from_slice(&app_data.raw_data.to_vec()) {
                            Ok(deserialized)  => {
                                deserialized
                            },
                            Err(_) => {
                                debug!("Unable to deserialize into data wrapper {:?}", app_data.raw_data);
                                continue
                            }
                        };

                        debug!("Wrapped data : {:?}", wrapped_data);

                        match wrapped_data.discriminator {

                            //  Handle a message designated as a Relay message
                            //
                            DcDataDiscriminator::Relay(route_info) => {

                                //  Check the destination againt the identifiers this application is representing
                                let identifiers_guard = match identifiers_arc.lock() {
                                    Ok(guard) => guard,
                                    Err(poisoned) => poisoned.into_inner(),
                                };

                                let mut message_for_us : bool = false;
                                'id_loop: for id in identifiers_guard.iter() {
                                    if id.to_string() == route_info.node_destination {
                                        message_for_us = true;
                                        break 'id_loop;
                                    }
                                }

                                drop(identifiers_guard);

                                // If the message is for us, then form a CarbonMsg and send it to the application
                                if message_for_us {

                                    if let Err(_) = app_data_sender.send(system::CarbonMsg{

                                        origin: system::MessageHeader{
                                            peer_id: app_data.peer_id.clone(),
                                            network: route_info.application_origin.clone()
                                        },

                                        raw_data: wrapped_data.raw_data.clone()
                                    }){
                                        warn!("Error sending data to app_data_sender channel [relay]");
                                    }

                                // If the message isn't for us, then check to see if we are a acting as a relay. If we are, push it out
                                // If we aren't, then we'll drop the message
                                } else {

                                    // Check if designated as relay. If so, handle the relay messaage, otherwise, drop it
                                    let carbon_app_guard = match carbon_app_arc.lock() {
                                        Ok(guard) => guard,
                                        Err(poisoned) => poisoned.into_inner(),
                                    };

                                    if carbon_app_guard.is_bridge {

                                        warn!("Need to relay message, but it isn't completed yet!");

/*
    -----------------------------------------------------------------------------------------------------------------------------
                            TODO:   This will need to be done during the Data-Handling branch / task

                                            Check to see if the node destination network is one this node sits on
                                            If it is, direct send it 

                                            Otherwise, drop it. Dropping it means that messages can only be relayed up-to 1 
                                            degree of separation. If relay messages are altered to place hop info, then we _COULD_
                                            relay until the hop counter reaches 0. ... realistically this is the best idea.

                    ====> DO NOT BROADCAST RELAY MESSAGES. Otherwise they will ripple around the network forever <====
    -----------------------------------------------------------------------------------------------------------------------------                              
*/

                                    } else {
                                        info!("Asked to relay message, but not designated to handle that. Dropping");
                                    }
                                    drop(carbon_app_guard);
                                }
                            },

                            //  Handle a message designated as a Query message
                            //
                            DcDataDiscriminator::Query(query_info) => {

                                debug!("Query: {:?}", query_info);
/*
    -----------------------------------------------------------------------------------------------------------------------------

                    TODO:   This will need to be done during the Data-Handling branch / task

   -----------------------------------------------------------------------------------------------------------------------------
*/
                            },

                            //  Handle a message designated as a Broadcast message
                            //
                            DcDataDiscriminator::Broadcast(origin_network) => {

                                if let Err(_) = app_data_sender.send(system::CarbonMsg{

                                    origin: system::MessageHeader{
                                        peer_id: app_data.peer_id.clone(),
                                        network: origin_network.clone()
                                    },

                                    raw_data: wrapped_data.raw_data.clone()
                                }){
                                    warn!("Error sending data to app_data_sender channel [broadcast]");
                                }
                            },

                            //  Handle a message designated as a PointToPoint message
                            //
                            DcDataDiscriminator::PointToPoint => {

                                if let Err(_) = app_data_sender.send(system::CarbonMsg{

                                    origin: system::MessageHeader{
                                        peer_id: app_data.peer_id.clone(),
                                        network: "local".to_string()
                                    },

                                    raw_data: wrapped_data.raw_data.clone()
                                }){
                                    warn!("Error sending data to app_data_sender channel [broadcast]");
                                }
                            }
                        }
                    }
                }

                // ------------------------------------------- Check data outbound channels for sending through to nexus -------------------------------------------

                for broadcast_message in broadcast_receiver.try_recv() {

                    let encoded = match serde_json::to_vec(&broadcast_message) {
                        Ok(enc) => enc,
                        Err(_)  => {
                            debug!("Unable to serialize broadcast message");
                            continue;
                        }
                    };
                    
                    for manager in netman_guard.iter_mut() {
                        if let Err(_) = manager.app_broadcast_message(encoded.to_vec()) {
                            debug!("Unable to broadcast on network with id: {:?}", manager.app_get_manager_id());
                        }
                    }
                }

                for relay_message in relay_reciever.try_recv() {

                    for manager in netman_guard.iter_mut() {

                        if manager.app_get_manager_id().to_string() == relay_message.network_destination.to_string() {

                            let encoded = match serde_json::to_vec(&relay_message.wrapped_data) {
                                Ok(enc) => enc,
                                Err(_)  => {
                                    debug!("Unable to serialize relay message");
                                    continue;
                                }
                            };

                            if let Err(_) = manager.app_direct_send(&relay_message.node_destination.to_string(), encoded.to_vec()) {
                                debug!("Unable to direct send message in relay_message");
                            }
                        }
                    }
                }

                for direct_message in direct_receiver.try_recv() {

                    for manager in netman_guard.iter_mut() {

                        if manager.app_get_manager_id().to_string() == direct_message.network_destination.to_string() {

                            let encoded = match serde_json::to_vec(&direct_message.wrapped_data) {
                                Ok(enc) => enc,
                                Err(_)  => {
                                    debug!("Unable to serialize relay message");
                                    continue;
                                }
                            };

                            if let Err(_) = manager.app_direct_send(&direct_message.node_destination.to_string(), encoded.to_vec()) {
                                debug!("Unable to direct send message in direct_message");
                            }
                        }
                    }
                }

                drop(netman_guard);
                thread::sleep(time::Duration::from_millis(100));
            }


        }));

        self.nexus_input_handler = thread_handler;
        Ok(())
    }
}

//
/// Initialize the application and receive a control panel interface for the application
//
pub fn init_application(configuration_file: String) -> Result<ControlPanel, &'static str> {
    //                                                                                                             //
    //  -------------------------------------- Load the application files ---------------------------------------  //
    //                                                                                                             //

    // Load the application from the configuration file
    let application = match system::app_from_file(&configuration_file) {
        Some(app) => {
            app
        },
        None => {
            return Err("Error loading application from file");
        }
    };

    let (app_sender,         app_receiver):       (CrossSend<system::CarbonMsg>, CrossRecv<system::CarbonMsg>) = unbounded();
    let (broadcast_sender,   broadcast_receiver): (CrossSend<DcDataWrapper>,     CrossRecv<DcDataWrapper>) = unbounded();
    let (relay_sender,       relay_receiver):     (CrossSend<DcSendable>,        CrossRecv<DcSendable>) = unbounded();
    let (direct_sender,      direct_receiver):    (CrossSend<DcSendable>,        CrossRecv<DcSendable>) = unbounded();

    // Create the control panel
    let mut control_panel : ControlPanel = ControlPanel {
        app_running:            Arc::new(AtomicBool::new(false)),
        network_managers:       Arc::new(Mutex::new(Vec::new())),
        local_arcs:             RefCell::new(Vec::new()),
        identifiers:            Arc::new(Mutex::new(Vec::new())),
        network_map:            Arc::new(Mutex::new(HashMap::new())),
        nexus_input_handler:    None,
        nexus_update_handler:   None,
        carbon_app:             Arc::new(Mutex::new(application.clone())),
        app_data_receiver:      app_receiver,
        app_data_sender:        app_sender,
        broadcast_outbound:     broadcast_receiver,
        broadcast_sender:       broadcast_sender,
        relay_outbound:         relay_receiver,
        relay_sender:           relay_sender,
        direct_send_outbound:   direct_receiver,
        direct_send_sender:     direct_sender,  
    };

    let mut collected_identifiers : Vec<String> = Vec::new();

    // Create all of the network managers based on the network information contained in the application connections
    for nexus_connection in application.connections {

        // Read in the local node
        let connections_local_node : nexus::system::Local = match nexus::system::local_from_file(&nexus_connection.nexus_local_file) {
            Some(local_node) => {
                local_node
            },
            None => {
                warn!("Unable to load local node from file '{}' ", nexus_connection.nexus_local_file);
                return Err("Error loading a local node");
            }
        };

        collected_identifiers.push(connections_local_node.id.clone());

        // Wrap the local node in an arc, and add it to the arcs
        control_panel.local_arcs.borrow_mut().push(
            Arc::new(Mutex::new(connections_local_node.clone()))
        );

        let arc_len = control_panel.local_arcs.borrow().len()-1;
        let current_arc = match control_panel.local_arcs.borrow().get(arc_len) {
            Some(arc) => {
                Arc::clone(arc)
            },
            None => {
                return Err("Error getting an arc clone for the currently loaded arc");
            }
        };

        let connections_network_config : nexus::system::Network = match nexus::system::network_from_file(&nexus_connection.nexus_network_file) {
            Some(network) => {
                network
            },
            None => {
                warn!("Unable to load network config from file '{}' ", nexus_connection.nexus_network_file);
                return Err("Error loading a network configuration");
            }
        };

        let network_manager : nexus::network::NetworkManager = nexus::network::get_network_manager(&nexus_connection.nexus_proto, 
                                                                                                         &Arc::clone(&current_arc), 
                                                                                                         &connections_network_config.clone());
        
        // Create the network map that the user can pull and use
        let mut netmap_guard = match control_panel.network_map.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };
        netmap_guard.insert(network_manager.app_get_manager_id(), network_manager.app_get_peers());
        drop(netmap_guard);

        // Add the network manager to the network manager list
        let mut netman_guard = match control_panel.network_managers.lock() {
            Ok(guard) => guard,
            Err(poisoned) => poisoned.into_inner(),
        };
        netman_guard.push(network_manager);
        drop(netman_guard);
    }

    control_panel.identifiers = Arc::new(Mutex::new(collected_identifiers.clone()));

    Ok(control_panel)
}