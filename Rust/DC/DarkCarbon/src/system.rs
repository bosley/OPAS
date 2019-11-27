use std::fs::File;
use std::path::Path;
use std::io::{Read, Write};

use serde::{Deserialize, Serialize};

extern crate uuid;
use uuid::Uuid;
//
/// Connection information for a single nexus network. Only one local file required for each network as 
/// all nodes known to the nexus node in the file will reachable. The DC system sill sit on-top of this node
//
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct NexusConnectionInfo {
    pub nexus_local_file:    String,                           // nexus::system::Local file - Will use nodes known to this file ,
    pub nexus_network_file:  String,                           // nexus::system::Network file,
    pub nexus_proto:         nexus::network::TransferProtocol  // nexus::network::TransferProtocol
}

//
/// Dark Carbon Application
//
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct CarbonApp {
    pub id:          String,                   // Unique id marking the app
    pub name:        String,                   // Name of the application
    pub tags:        Vec<String>,              // Tags given to the application
    pub description: String,                   // Description of the application
    pub is_bridge:   bool,                     // App should perform bridging of given network
    pub connections: Vec<NexusConnectionInfo>, // Nexus connections
    pub file_name:   String,                   // File to write out to
}

//
/// Information regarding a remote peer for data routing
//
#[derive(Clone, Debug)]
pub struct MessageHeader {
    pub peer_id:  String,                
    pub network:  String,
}

//
/// A message from a nexus connection by-way-of the Dark Carbon library to the application 
//
#[derive(Clone, Debug)]
pub struct CarbonMsg {
    pub origin:          MessageHeader,
    pub raw_data:        Vec<u8>
}

//
/// Create a brand-new application from information provided.
//
pub fn app_from_info(name:        String, 
                     tags:        Vec<String>, 
                     description: String, 
                     is_bridge:   bool, 
                     connections: Vec<NexusConnectionInfo>, 
                     file_name:   String) -> CarbonApp {

    CarbonApp{
        id:          Uuid::new_v4().to_hyphenated_ref().to_string(),
        name:        name,
        tags:        tags,
        description: description,
        is_bridge:   is_bridge,
        connections: connections,
        file_name:   file_name
    }
}

//
/// Load application from file
//
pub fn app_from_file(file: &String) -> Option<CarbonApp> {

    let buff : String = match from_file(file) {
        Some(s) => s,
        None    => return None
    };

    let app : CarbonApp = match serde_json::from_str(&buff) {
        Ok(node) => node,
        Err(_)   => {
            warn!("Error deserializing app from file");
            return None;
        }
    };

    Some(app)
}

// Read a string from a file
fn from_file(file: &String) -> Option<String> {
    
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

// Write data to a file
fn write_to_file(data: &String, file: &String) -> Result<(), &'static str> {

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

impl CarbonApp {

    pub fn write(self: &Self) -> Result<(), &'static str> {

        let data = match serde_json::to_string_pretty(self) {
            Err(_) => return Err("Unable to serialize local node data"),
            Ok(d)  => d.to_string()
        };

        return write_to_file(&data.to_string(), &self.file_name.to_string());
    }
}
