extern crate rna;
extern crate serde;

extern crate crypto;
use crypto::digest::Digest;
use crypto::sha3::Sha3;

extern crate chrono;

use crate::common;
use crate::datastore;

use rna::tcplib::TcpReceiver;
use std::sync::{ Arc, Mutex };
use std::sync::atomic::{AtomicBool, Ordering};
use std::net::{ TcpListener, TcpStream };
use std::thread;
use std::time;
use std::io::ErrorKind;
use std::collections::HashMap;
use threadpool::ThreadPool;
use crossbeam::crossbeam_channel::{ Receiver, Sender, unbounded};

// A tcp receiver struct to pass off as an Arc to TCP threads when data comes in
struct GatewayTcpReceiver{

    gateway_channel_sender:   Sender<common::GatewayTransfer>,  // Pass aggregated data to be processed
    aggregated_data:          Vec<u8>                           // Place to aggregate all inbound data
}

// ---------------------------------------------------------------------------
// Create a new tcp receiver
// ---------------------------------------------------------------------------

fn new_gatway_tcp_receiver(gateway_channel_sender: Sender<common::GatewayTransfer>) -> GatewayTcpReceiver{
    GatewayTcpReceiver{
        gateway_channel_sender: gateway_channel_sender,
        aggregated_data:        Vec::new()
    }
}

impl TcpReceiver for GatewayTcpReceiver {
    fn maximum_bytes(&self) -> u64{
        2_000_000   // Limit data transfer to 2MB per message
    }

    fn begin_receive(&mut self, _id: &String) {
        //debug!("New receive started with id: {}", id);
    }

    fn bytes_received(&mut self, _id: &String, data: &Vec<u8>) {
        //debug!("Gateway receiver got : {} bytes for stream {} ", data.len(), id);
        self.aggregated_data.extend(data.clone());
    }

    fn receive_complete(&mut self, _id: &String) {
        //debug!("All data for {} has been received", id);

        let deserialized_data : common::GatewayTransfer =  match serde_json::from_slice(
            self.aggregated_data.as_ref()
        ){
            Ok(deserialized) => {
                deserialized
            },
            Err(e) => {
                warn!("Unable to deserialize vector data with length {}. Error {}", 
                        self.aggregated_data.len(),
                        e);
                return;
            }
        };

        if let Err(e) = self.gateway_channel_sender.send(deserialized_data.clone()) {
            warn!("Error placing aggregated data into gateway [{}]'s sender", e);
        }
    }
}

pub struct Gateway {

    // Connection and state information
    connection:               String,
    id:                       String,
    running:                  Arc<AtomicBool>,
    name:                     String,

    // Remote gateways                            id       gateway information
    known_gateways:           Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,

    // Nodes on the same network as the gateway   id       node information
    local_nodes:              Arc<Mutex<HashMap<String, common::RemoteNodeInformation>>>,

    // Gateway threading
    server_handle:            Option<thread::JoinHandle<()>>,
    gateway_thread:           Option<thread::JoinHandle<()>>,
    tcp_thread_pool:          ThreadPool,

    // Gateway channels
    gateway_channel_receiver: Receiver<common::GatewayTransfer>, 
    gateway_channel_sender:   Sender<common::GatewayTransfer>, 
}

fn create_gateway(name: String, id: String, connection: String, thread_count: usize, 
                  known_gateways: HashMap<String, common::RemoteGatewayInformation>,
                  local_nodes:    HashMap<String, common::RemoteNodeInformation>) -> Gateway {

    let (gateway_sender,  gateway_receiver):  (Sender<common::GatewayTransfer>, Receiver<common::GatewayTransfer>) = unbounded();

    Gateway{

        // connection and state information
        connection:         connection,
        id:                 id,
        running:            Arc::new(AtomicBool::new(false)),
        name:               name,

        // Remote gateways
        known_gateways:     Arc::new(Mutex::new(known_gateways.clone())),

        // Nodes on the same network as the gateway
        local_nodes:        Arc::new(Mutex::new(local_nodes.clone())),

        // threading
        server_handle:      None,
        gateway_thread:     None,
        tcp_thread_pool:    ThreadPool::new(thread_count),

        // channels
        gateway_channel_receiver: gateway_receiver,
        gateway_channel_sender:   gateway_sender
    }
}

// ---------------------------------------------------------------------------
/// Create a new gateway - Meant only to generate gateway file
// ---------------------------------------------------------------------------

pub fn new_gateway(name: String) -> Gateway {
    create_gateway(name, 
        common::generate_id(), 
        "0.0.0.0:0000".to_string(), 
        1, 
        HashMap::new(),
        HashMap::new())
}

// ---------------------------------------------------------------------------
/// Load a gateway from file
// ---------------------------------------------------------------------------

pub fn load_gateway(name: String, ip: String, port: String, thread_count: usize) -> Result<Gateway, &'static str> {

    let gateway_format : datastore::GatewayFormat = datastore::gateway_load_and_convert(format!("{}.json", name))?;

    Ok(create_gateway(name,
        gateway_format.id,
        format!("{}:{}", ip, port), 
        thread_count, 
        gateway_format.remote_gateways.clone(), 
        gateway_format.local_nodes.clone()))
}

impl Gateway {

    // ---------------------------------------------------------------------------
    /// Start the gateway
    // ---------------------------------------------------------------------------

    pub fn serve(&mut self) -> Result<(), &'static str> {

        debug!("Gateway {} asked to serve on : {}", self.id, self.connection);

        // Ensure that the server is not running
        if self.running.load(Ordering::SeqCst) {
            return Err("Gateway already running");
        }

        self.running.store(true, Ordering::SeqCst);

        self.start_server_thread();
        self.start_gateway_thread();
        Ok(())
    }

    // ---------------------------------------------------------------------------
    /// Inject a remote gateway into local gateway
    // ---------------------------------------------------------------------------

    pub fn inject_remote_gateway(&mut self,id: String, conn: String) -> Result<(), &'static str> {

        let mut gate_guard    = match self.known_gateways.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        gate_guard.insert(
            id.clone(),
            common::RemoteGatewayInformation{
                id:     id,
                conn:   conn,
                hash:   "Unknown".to_string(),
                nodes:  Vec::new()}
            );

        drop(gate_guard);

        return self.save();
    }

    // ---------------------------------------------------------------------------
    /// Save the state of the gateway
    // ---------------------------------------------------------------------------

    pub fn save(&self) -> Result<(), &'static str> {

        let file_location = format!("{}.json", self.name);

        let gate_guard    = match self.known_gateways.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let gate_copy = gate_guard.clone();

        drop(gate_guard);

        let node_guard    = match self.local_nodes.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let node_copy = node_guard.clone();

        drop(node_guard);

        match datastore::gateway_convert_and_save(file_location, datastore::GatewayFormat{
            id:              self.id.clone(),
            remote_gateways: gate_copy,
            local_nodes:     node_copy
        }) {
            Ok(_) => {
                debug!("Gateway saved");
            },
            Err(e) => {
                debug!("Unable to save gateway : {}", e);
                return Err("Unable to save");
            }
        }

        Ok(())
    }

    // ---------------------------------------------------------------------------
    /// Stop the gateway
    // ---------------------------------------------------------------------------

    pub fn stop(&mut self) -> Result<(), &'static str> {

        debug!("Gateway {} asked to stop", self.id);

        // Ensure that the server is actually running
        if !self.running.load(Ordering::SeqCst) {
            return Err("Gateway not running");
        }

        // Indicate that the server is no longer suppose to run
        self.running.store(false, Ordering::SeqCst);

        // Join server thread
        self.server_handle
            .take().expect("Called stop on non-running thread")
             .join().expect("Could not join server thread");

        // Join gateway thead
        self.gateway_thread
            .take().expect("Called stop on non-running thread")
            .join().expect("Could not join gateway thread");

        Ok(())
    }

    // ---------------------------------------------------------------------------
    // Start the server thread
    // ---------------------------------------------------------------------------

    fn start_server_thread(&mut self) {

        // start the server thread, and place the handle in Gateway
        // use threadpool to spin off new connections, and 

        let connection_information = self.connection.clone();
        let arc_running            = Arc::clone(&self.running);
        let tcp_thread_pool        = self.tcp_thread_pool.clone();
        let gateway_sender         = self.gateway_channel_sender.clone();

        self.server_handle =  Some(thread::spawn(move || {

            let listener = match TcpListener::bind(String::clone(&connection_information)) {
                Ok(l)  => l,
                Err(e) => { panic!("Unable to bind TcpListener: {}", e) }
            };

            listener.set_ttl(1).expect("could not set TTL");
            listener.set_nonblocking(true).expect("Cannot set non-blocking");

            'listener: for stream in listener.incoming() {

                if !arc_running.load(Ordering::SeqCst) {
                    break 'listener;
                }

                let mut data_stream = match stream {
                    Ok(s) => { s }
                    Err(ref e) if e.kind() == ErrorKind::WouldBlock => {
                        continue;
                    }
                    Err(_) => {
                        error!("Io Error");
                        break 'listener;
                    }
                };

                if let Err(e) = data_stream.set_read_timeout(
                    Some(time::Duration::from_secs(10))) {
                        warn!("Unable to set read timeout on new stream: {}", e);
                    }

                let gateway_sender_inner         = gateway_sender.clone();

                // Thread off the connection to get data
                tcp_thread_pool.execute(move|| {

                    let mut gateway_tcp = new_gatway_tcp_receiver(gateway_sender_inner.clone());
                    let mut tcp_inbound = rna::tcplib::TcpInbound::new(&mut gateway_tcp);

                    match tcp_inbound.read_stream(&mut data_stream) {
                        Ok(_) => {
                            debug!("tcp_inbound.read_stream() success");
                        },
                        Err(e) => {
                            warn!("{}", e);
                        }
                    }
                }); // End tcp_thread_pool
            } // End stream in listener
        })); // End server_handle
    }

    // ---------------------------------------------------------------------------
    // Start the gateway thread
    // ---------------------------------------------------------------------------

    fn start_gateway_thread(&mut self) {

        // start a thread to iterate over crossbeam channels that relate to gateway synchronization operations
        // place the handle in Gateway
        // these will be populated by the tcp receiver from a server thread

        let arc_running            = Arc::clone(&self.running);
        let gateway_receiver       = self.gateway_channel_receiver.clone();
        let arc_local_nodes        = self.local_nodes.clone();
        let arc_known_gateways     = self.known_gateways.clone();
        let local_id               = self.id.clone();
        let local_conn             = self.connection.clone();

        self.gateway_thread =  Some(thread::spawn(move || {

            let mut update_timer = chrono::Utc::now();

            while arc_running.load(Ordering::SeqCst) {

                // Pull gateway requests from channel and process them

                let gateway_items: Vec<common::GatewayTransfer> = gateway_receiver.try_iter().collect();

                let local_nodes     = arc_local_nodes.clone();
                let known_gateways  = arc_known_gateways.clone();
                let local_id_copy   = local_id.clone();
                let local_conn_copy = local_conn.clone();

                for item in gateway_items {   

                    // Determine what the item is for, and pipe it to the corresponding handler
                    match item.discriminator {

                        common::GatewayDiscriminator::HashRequest => {
                            handle_gateway_hash_request(local_id_copy.clone(),
                                                        local_conn_copy.clone(),
                                                        local_nodes.clone(), 
                                                        known_gateways.clone(), 
                                                        item.data);
                        },

                        common::GatewayDiscriminator::HashResponse => {
                            handle_gateway_hash_response(local_id_copy.clone(),
                                                         local_conn_copy.clone(),
                                                         known_gateways.clone(), 
                                                         item.data);
                        },

                        common::GatewayDiscriminator::MapRequest => {
                            handle_gateway_map_request(local_id_copy.clone(),
                                                       local_conn_copy.clone(),
                                                       local_nodes.clone(), 
                                                       known_gateways.clone(), 
                                                       item.data);
                        },

                        common::GatewayDiscriminator::MapResponse => {
                            handle_gateway_map_response(known_gateways.clone(), 
                                                        item.data);
                        },

                        common::GatewayDiscriminator::GatewayToGatewayTransfer => {
                            handle_gateway_to_gateway_transfer(local_nodes.clone(),
                                                               item.data);
                        },

                        common::GatewayDiscriminator::NodeToNodeTransfer => {
                            handle_node_to_node_transfer(local_id_copy.clone(),
                                                         known_gateways.clone(),
                                                         item.data);
                        }
                    }
                }

                //
                //  Iterate over known gateways and send-out hash requests. If it is time.
                //
                if chrono::Utc::now() - update_timer > chrono::Duration::minutes(1) {

                    debug!("Running updater");

                    update_timer = chrono::Utc::now(); // Set for now, if an error comes up retry next time

                    // Kick off update by sending a hash request. When they send a hash response back
                    // The hashes will be checked. If they do not match, a map request is sent, and we
                    // will be updated once they send a map response.

                    let hash_request : common::HashRequest = common::HashRequest{
                        caller_id:   local_id_copy.clone(),
                        caller_conn: local_conn_copy.clone()
                    };

                    let serialized_request = match serde_json::to_vec(&hash_request) {
                        Ok(serialized) => {
                            serialized
                        },
                        Err(e) => {
                            warn!("Unable to serialize hash request in gateway timed update : {}", e);
                            continue;
                        }
                    };

                    let transfer : common::GatewayTransfer = common::GatewayTransfer {
                        discriminator: common::GatewayDiscriminator::HashRequest,
                        data:          serialized_request.clone()
                    };

                    let serialized_transfer = match serde_json::to_vec(&transfer) {
                        Ok(serialized) => {
                            serialized
                        },
                        Err(e) => {
                            warn!("Unable to serialize transfer in gateway timed update : {}", e);
                            continue;
                        }
                    };

                    // Iterate over all known gateways and send the request

                    let gate_guard    = match known_gateways.lock() {
                        Ok(guard)     => guard,
                        Err(poisoned) => poisoned.into_inner(),
                    };

                    // Keys will be ids
                    for (key, val) in gate_guard.iter() {

                        debug!("Sending serialized hash request to {}", key);

                        match tcp_send(val.conn.clone(), serialized_transfer.clone()) {
                            Ok(_) => {
                                debug!("Sent map request!");
                            },
                            Err(e) => {
                                warn!("Unable to send map request : {}", e);
                            }
                        }
                    }
                
                    drop(gate_guard);

                    debug!("Updater complete");
                }

                thread::sleep(time::Duration::from_millis(50));
            } // End while running
        })); // End gateway_thread
    }
}

// ###########################################################################
//
//          Gateway response / request handlers
//
// ###########################################################################

// ---------------------------------------------------------------------------
// Handle a gateway hash request
// ---------------------------------------------------------------------------

fn handle_gateway_hash_request(
    local_id:   String,
    local_conn: String,
    nodes:      Arc<Mutex<HashMap<String, common::RemoteNodeInformation>>>,
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,
    request:    Vec<u8>) {

    // Deserialize request into hashrequest
    let deserialized_request : common::HashRequest =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize hash request in handle_gateway_hash_request! {}", e);
            return;
        }
    };

    let hash = match obtain_hash(nodes.clone(), gateways.clone()){
        Ok(the_hash) => {
            the_hash
        }, 
        Err(e) => {
            warn!("Unable to caclulate hash in handle_gateway_hash_request : {}", e);
            return;
        }
    };

    debug!("Hex result of hasher in hash request: {}", hash);

    debug!("Sending hash to {}", deserialized_request.caller_id);

    // Make and serialize the response
    let response : common::HashResponse = common::HashResponse{
        caller_id:      local_id,
        caller_conn:    local_conn,
        hash:           hash
    };

    let serialized_response = match serde_json::to_vec(&response) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize hash response in handle_gateway_hash_request : {}", e);
            return;
        }
    };

    // Wrap it in a transfer and serialize
    let transfer : common::GatewayTransfer = common::GatewayTransfer {
        discriminator: common::GatewayDiscriminator::HashResponse,
        data:          serialized_response.clone()
    };

    let serialized_transfer = match serde_json::to_vec(&transfer) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize transfer in gateway timed update : {}", e);
            return;
        }
    };

    // Send
    match tcp_send(deserialized_request.caller_conn, serialized_transfer) {
        Ok(_) => {
            debug!("Sent hash response!");
        },
        Err(e) => {
            warn!("Unable to send hash response : {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
// Handle a gateway hash response
// ---------------------------------------------------------------------------

fn handle_gateway_hash_response(
    local_id:   String,
    local_conn: String,
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,
    request:    Vec<u8>) {

    // Deserialize request into hashresponse
    let deserialized_response : common::HashResponse =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize hash response in handle_gateway_hash_response! {}", e);
            return;
        }
    };

    debug!("Hash response from {}, they gave us {}", deserialized_response.caller_id, deserialized_response.hash);

    let gate_guard    = match gateways.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    if !gate_guard.contains_key(&deserialized_response.caller_id) {
        warn!("Received hash response from a gateway that is unknown [{}]", deserialized_response.caller_id);
        return;
    }

    let caller_gate = match gate_guard.get(&deserialized_response.caller_id) {
        Some(caller) => {
            caller
        }, 
        None => {
            warn!("Unable to get caller from gate hash map");
            return;
        }
    };

    if caller_gate.hash == deserialized_response.hash {
        debug!("Hash response caller's hash has matched our copy of their hash, so no updates need to be done");
        return;
    }

    // Since the hashes are different, we now need to request their information for synchronization

    let map_request : common::MapRequest = common::MapRequest {
        caller_id:   local_id,
        caller_conn: local_conn
    };

    let serialized_map_request = match serde_json::to_vec(&map_request) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize map request in handle_gateway_hash_response : {}", e);
            return;
        }
    };

    // Wrap it in a transfer and serialize
    let transfer : common::GatewayTransfer = common::GatewayTransfer {
        discriminator: common::GatewayDiscriminator::MapRequest,
        data:          serialized_map_request.clone()
    };

    let serialized_transfer = match serde_json::to_vec(&transfer) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize map request transfer : {}", e);
            return;
        }
    };

    match tcp_send(deserialized_response.caller_conn, serialized_transfer) {
        Ok(_) => {
            debug!("Sent map request!");
        },
        Err(e) => {
            warn!("Unable to send map request : {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
// Handle a gateway map request
// ---------------------------------------------------------------------------

fn handle_gateway_map_request(
    local_id:   String,
    local_conn: String,
    nodes:      Arc<Mutex<HashMap<String, common::RemoteNodeInformation>>>,
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,
    request:    Vec<u8>) {

    // Deserialize request into map request
    let deserialized_response : common::MapRequest =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize map request in handle_gateway_map_request! {}", e);
            return;
        }
    };

    // Get our current hash
    let hash = match obtain_hash(nodes.clone(), gateways.clone()){
        Ok(the_hash) => {
            the_hash
        }, 
        Err(e) => {
            warn!("Unable to caclulate hash in handle_gateway_map_request : {}", e);
            return;
        }
    };

    // Make a vector of our nodes
    let mut node_vec : Vec<common::RemoteNodeInformation> = Vec::new();

    let node_guard    = match nodes.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    for (_key, val) in node_guard.iter() {
        node_vec.push(val.clone());
    }

    drop(node_guard);

    // Create a RemoteGatewayInformation struct for them to use in reaching us
    let our_remote_gateway_info = common::RemoteGatewayInformation{
        id:     local_id.clone(),
        conn:   local_conn.clone(),
        hash:   hash,
        nodes:  node_vec
    };

    // Create the map response
    let map_response : common::MapResponse = common::MapResponse {
        caller_id: local_id.clone(),
        map:       our_remote_gateway_info
    };

    // Serialize it
    let serialized_map_response = match serde_json::to_vec(&map_response) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize map response in handle_gateway_map_request : {}", e);
            return;
        }
    };

    // Wrap it in a transfer and serialize
    let transfer : common::GatewayTransfer = common::GatewayTransfer {
        discriminator: common::GatewayDiscriminator::MapResponse,
        data:          serialized_map_response.clone()
    };

    let serialized_transfer = match serde_json::to_vec(&transfer) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize map response transfer : {}", e);
            return;
        }
    };

    // send it
    match tcp_send(deserialized_response.caller_conn, serialized_transfer) {
        Ok(_) => {
            debug!("Sent map request!");
        },
        Err(e) => {
            warn!("Unable to send map request : {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
// Handle a gateway map response
// ---------------------------------------------------------------------------

fn handle_gateway_map_response(
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,
    request: Vec<u8>) {

    // Deserialize request into map response
    let deserialized_response : common::MapResponse =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize map request in handle_gateway_map_request! {}", e);
            return;
        }
    };

    let mut gate_guard    = match gateways.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    gate_guard.insert(deserialized_response.caller_id, deserialized_response.map);

    drop(gate_guard);

    debug!("Map response processed");
}

// ---------------------------------------------------------------------------
// Handle a gateway to gateway transfer 
// ---------------------------------------------------------------------------

fn handle_gateway_to_gateway_transfer(
    local_nodes:   Arc<Mutex<HashMap<String, common::RemoteNodeInformation>>>,
    request:       Vec<u8>) {

    // Deserialize request into map response
    let deserialized_transfer : common::GatewayToGatewayTransfer =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize g2g transfer in handle_gateway_to_gateway_transfer! {}", e);
            return;
        }
    };

    debug!("Got a transfer containing node data from {}, meant for {}", 
            deserialized_transfer.origin_gate, 
            deserialized_transfer.transfer_info.origin_node);

    // Find local node and send the transfer_info if they are indeed existing

    let node_guard    = match local_nodes.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    if !node_guard.contains_key(&deserialized_transfer.transfer_info.destination_node) {
        warn!("Remote gateway {} attempting to send to local node {} that doesn't exist",
              deserialized_transfer.origin_gate,
              deserialized_transfer.transfer_info.destination_node);
        return;
    }

    let mut destination_connection : String = String::new();
    'search: for (key, value) in node_guard.iter() {

        if key == &deserialized_transfer.transfer_info.destination_node {
            destination_connection = value.conn.clone();
            break 'search;
        }
    }

    drop(node_guard);

    if destination_connection.len() == 0 {
        warn!("Something weird happened. An existing node {} wasn't found. =[",
              deserialized_transfer.transfer_info.destination_node);
        return;
    }

    let serialized_transfer = match serde_json::to_vec(&deserialized_transfer.transfer_info) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize NodeToNodeTransfer in g2g transfer : {}", e);
            return;
        }
    };

    match tcp_send(destination_connection, serialized_transfer) {
        Ok(_) => {
            debug!("Sent data to node!");
        },
        Err(e) => {
            warn!("Unable to send data to node : {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
// Handle a gateway to node transfer
// ---------------------------------------------------------------------------

fn handle_node_to_node_transfer(
    local_id:   String,
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>,
    request:    Vec<u8>) {

    // Data from node meant for a remote node

    // Deserialize request into map response
    let deserialized_transfer : common::NodeToNodeTransfer =  match serde_json::from_slice(
        request.as_ref()
    ){
        Ok(deserialized) => {
            deserialized
        },
        Err(e) => {
            warn!("Unable to deserialize n2n transfer in handle_node_to_node_transfer! {}", e);
            return;
        }
    };

    // Need to find the remote gateway that contains the intended node. 
    let gate_guard    = match gateways.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    let mut gateway_conn :  String = String::new();
    'outer: for (key, value) in gate_guard.iter() {
        for item in value.nodes.iter() {
            if item.id == deserialized_transfer.destination_node {
                debug!("Found the destination node in gateway {}'s node list ", key);
                gateway_conn = value.conn.clone();
                break 'outer;
            }
        }
    }

    drop(gate_guard);

    // Create g2g transfer
    let g2g : common::GatewayToGatewayTransfer = common::GatewayToGatewayTransfer {
        origin_gate:   local_id,
        transfer_info: deserialized_transfer.clone()
    };

    // Serialize it 
    let serialized_transfer = match serde_json::to_vec(&g2g) {
        Ok(serialized) => {
            serialized
        },
        Err(e) => {
            warn!("Unable to serialize GatewayToGatewayTransfer in n2n transfer : {}", e);
            return;
        }
    };

    // Send it
    match tcp_send(gateway_conn, serialized_transfer) {
        Ok(_) => {
            debug!("Sent data to gateway!");
        },
        Err(e) => {
            warn!("Unable to send data to gateway : {}", e);
        }
    }
}

// ###########################################################################
//
//          Helper methods
//
// ###########################################################################

// ---------------------------------------------------------------------------
// Send data to a remote
// ---------------------------------------------------------------------------

fn tcp_send(connection: String, data: Vec<u8>) -> Result<(), &'static str> {

    let mut stream = match TcpStream::connect(connection.clone()){
        Ok(s) => s,
        Err(e) => {
            warn!("Could not connect to {} : {}", connection, e); 
            return Err("Could not connect to remote");
        }
    };

    match rna::tcplib::writer(&mut stream, data) {
        Ok(_) => {
            debug!("Success sending data to {}", connection);
        },

        Err(_) => {
            return Err("Error sending data");
        }
    }

    Ok(())
}

// ---------------------------------------------------------------------------
// Calculate a hash of nodes + gateways
// ---------------------------------------------------------------------------

fn obtain_hash(
    nodes:      Arc<Mutex<HashMap<String, common::RemoteNodeInformation>>>,
    gateways:   Arc<Mutex<HashMap<String, common::RemoteGatewayInformation>>>) -> Result<String, &'static str>
{
    // create a SHA3-256 object
    let mut hasher = Sha3::sha3_256();

    let node_guard    = match nodes.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    // Keys will be ids
    for (key, _val) in node_guard.iter() {

        let encoded_key = match serde_json::to_vec(key) {
            Ok(encoded) => {
                encoded
            }, 
            Err(_) => {
                return Err("Unable to encode key for node in hash calculation");
            }
        };

        hasher.input(&encoded_key[..]);
    }

    drop(node_guard);

    let gate_guard    = match gateways.lock() {
        Ok(guard)     => guard,
        Err(poisoned) => poisoned.into_inner(),
    };

    // Keys will be ids
    for (key, _val) in gate_guard.iter() {

        let encoded_key = match serde_json::to_vec(key) {
            Ok(encoded) => {
                encoded
            }, 
            Err(_) => {
                return Err("Unable to encode key for gateway in hash calculation");
            }
        };

        hasher.input(&encoded_key[..]);
    }

    drop(gate_guard);

    hasher.input_str("Ensure it isn't empty or something");
    
    Ok(hasher.result_str())
}