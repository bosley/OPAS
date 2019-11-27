extern crate uuid;
use uuid::Uuid;

extern crate serde;
use serde::{Deserialize, Serialize};

use std::fs::File;
use std::path::Path;
use std::io::{Read, Write};

/// Generate a unique id 
pub fn generate_id() -> String {

    let uuid = Uuid::new_v4().to_hyphenated_ref().to_string();
    
    match uuid.split("-").next() {
        Some(id) => {
            id.to_string()
        },
        None => {
            uuid
        }
    }
}

// ---------------------------------------------------------------------------
// Gateway messages 
// ---------------------------------------------------------------------------

/*
    local_gateway  -{HashRequest}-> remote_gateway
    local_gateway <-{HashResponse}- remote_gateway

    local_gateway  -{MapRequest}-> remote_gateway
    local_gateway <-{MapResponse}- remote_gateway

    local_node  -{NodeToNodeTransfer}-> local_gateway  -{GatewayToGatewayTransfer}-> remote_gateway  -{NodeToNodeTransfer}-> remote_node
    local_node <-{NodeToNodeTransfer}-  local_gateway <-{GatewayToGatewayTransfer}-  remote_gateway <-{NodeToNodeTransfer}-  remote_node

*/

// ---------------------------------------------------------------------------

/// Enum to determine the contents stored in gateway transfer
#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum GatewayDiscriminator {

    HashRequest,        // From / to gateway
    HashResponse,       // From / to gateway

    MapRequest,         // From / to gateway
    MapResponse,        // From / to gateway

    // Node data transmission
    GatewayToGatewayTransfer,   // From node to gateway
    NodeToNodeTransfer          // From / to gateway
}

/// Structure to pass along gateway data
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GatewayTransfer {
    pub discriminator : GatewayDiscriminator,
    pub data:           Vec<u8>
}

/// A request for a hash of a gateways network map
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct HashRequest {
    pub caller_id:   String,
    pub caller_conn: String,
}

/// Response to HashRequest containing gateway network map hash
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct HashResponse {
    pub caller_id:    String,
    pub caller_conn:  String,
    pub hash:         String
}

/// A request for a gateways network map
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct MapRequest {
    pub caller_id:   String,
    pub caller_conn: String,
}

/// Response to HashRequest containing gateway network map hash
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct MapResponse {
    pub caller_id:    String,
    pub map:          RemoteGatewayInformation
}

/// Error information for node reporting
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct NodeTransferError{
    code: u64,
    description: String
}

/// Information discriminator for NodeToNode Transfer
#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum NodeToNodeDiscriminator {
    Request,
    Response,
    Error(NodeTransferError)
}

/// Transfer data from node to a remote node by-way-of a local gateway
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct NodeToNodeTransfer{
    pub event_id:         String,   // An event id spawned my caller so they can identify response
    pub origin_node:      String,   // Node of origin
    pub destination_node: String,   // Node destination
    pub discriminator:    NodeToNodeDiscriminator,
    pub data:             Vec<u8>   // Data to send
}

/// Transfer node data from gateway to gateway for remote node
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GatewayToGatewayTransfer{
    pub origin_gate:      String,
    pub transfer_info:    NodeToNodeTransfer
}

/// Node response to request from one gateway to another (initiated from a node local to sender)
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct RemoteNodeInformation{
    pub id:     String,
    pub conn:   String
}

// Information about a remote gateway
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct RemoteGatewayInformation {
    pub id:     String,     // id
    pub conn:   String,     // Connection
    pub hash:   String,     // Our stored hash of their information (nodes+gateways)
    pub nodes:  Vec<RemoteNodeInformation>  // Their nodes
}

// ---------------------------------------------------------------------------
/// Write data to file
// ---------------------------------------------------------------------------

pub fn write_to_file(data: &String, file: &String) -> Result<(), &'static str> {

    let path = Path::new(file);
    let mut o = match File::create(&path) {
        Err(_)   => return Err("Unable to create file"),
        Ok(file) => file,
    };

    if let Err(_) = o.write_all(data.as_bytes()) {
        return Err("Unable to write file");
    }
    drop(o);

    Ok(())
}

// ---------------------------------------------------------------------------
/// Read data from file
// ---------------------------------------------------------------------------

pub fn from_file(file: &String) -> Option<String> {
    
    let path = Path::new(file);

    let mut o = match File::open(&path){
        Ok(o)  => o,
        Err(_) => {
            warn!("Error opening file");
            return None;
        }
    };

    let mut buff = String::new();
    if let Err(_) = o.read_to_string(&mut buff){
        warn!("Error reading node from file");
        return None;
    };
    drop(o);

    Some(buff)
}