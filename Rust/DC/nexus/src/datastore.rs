
extern crate serde;
use serde::{Deserialize, Serialize};

use std::collections::HashMap;

use crate::common;

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GatewayDataStore {
    id:              String,
    remote_gateways: Vec<common::RemoteGatewayInformation>,
    local_nodes:     Vec<common::RemoteNodeInformation>
}

/// Used as an intermediary between a gateway and the datastore
#[derive(Clone, Debug)]
pub struct GatewayFormat {
    pub id:              String,
    pub remote_gateways: HashMap<String, common::RemoteGatewayInformation>,
    pub local_nodes:     HashMap<String, common::RemoteNodeInformation>
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct NodeDataStore {
    pub id:         String,
    pub name:       String,
    pub desc:       String,
    pub connection: String,
    pub gateway:    String
}

// ---------------------------------------------------------------------------
/// Save a node data store
// ---------------------------------------------------------------------------

pub fn node_save(file_location: String, node_ds: &NodeDataStore) -> Result<(), &'static str> {

    let serialized_data_store = match serde_json::to_string_pretty(&node_ds) {

        Ok(serialized) => {
            serialized
        },
        Err(or) => {
            warn!("Unable to serialize data store : {}", or);
            return Err("Unable to serialize data store");
        }
    };

    match common::write_to_file(&serialized_data_store, &file_location){
        Ok(_) => {
            debug!("Data written");
        },
        Err(or) => {
            warn!("Error putting data in file : {}", or);
            return Err(or);
        }
    }

    Ok(())
}

// ---------------------------------------------------------------------------
/// Load a node data store
// ---------------------------------------------------------------------------

pub fn node_load(file_location: String) -> Result<NodeDataStore, &'static str> {

    let data = match common::from_file(&file_location) {
        Some(d) => {
            d
        },
        None => {
            warn!("Error reading data from {}", file_location);
            return Err("Unable to load data from file ");
        }
    };

    let deserialized_data_store : NodeDataStore = match serde_json::from_str(&data) {
        Ok(data_store) => {
            data_store
        },
        Err(or) => {
            warn!("Unable to deserialize data store from {} : {}", file_location, or);
            return Err("Error deserializing data store");
        }
    };

    Ok(deserialized_data_store)
}

pub fn gateway_insert_node(file_location: String, node_ds: NodeDataStore) -> Result<(), &'static str> {

    let gate_data = match common::from_file(&file_location) {
        Some(d) => {
            d
        },
        None => {
            warn!("Error reading data from {}", file_location);
            return Err("Unable to load data from file ");
        }
    };

    let mut deserialized_data_store : GatewayDataStore = match serde_json::from_str(&gate_data) {
        Ok(data_store) => {
            data_store
        },
        Err(or) => {
            warn!("Unable to deserialize data store from {} : {}", file_location, or);
            return Err("Error deserializing data store");
        }
    };

    deserialized_data_store.local_nodes.push(
        common::RemoteNodeInformation{
            id: node_ds.id,
            conn: node_ds.connection
        }
    );

    let serialized_data_store = match serde_json::to_string_pretty(&deserialized_data_store) {

        Ok(serialized) => {
            serialized
        },
        Err(or) => {
            warn!("Unable to serialize data store : {}", or);
            return Err("Unable to serialize data store");
        }
    };
    
    match common::write_to_file(&serialized_data_store, &file_location){
        Ok(_) => {
            debug!("Data written");
        },
        Err(or) => {
            warn!("Error putting data in file : {}", or);
            return Err(or);
        }
    }

    Ok(())
}

// ---------------------------------------------------------------------------
/// Convert data from gateway format to datastore format and write to file
// ---------------------------------------------------------------------------

pub fn gateway_convert_and_save(file_location:  String,
                                gateway_format: GatewayFormat) -> Result<(), &'static str> {

    debug!("Running convert and save.. ");

    let mut ds : GatewayDataStore = GatewayDataStore {
        id:              gateway_format.id,
        remote_gateways: Vec::new(),
        local_nodes:     Vec::new()
    };

    debug!("Converting gateways");

    for (key, val) in gateway_format.remote_gateways.iter() {

        debug!("Converting gateway {}", key);

        ds.remote_gateways.push(val.clone());
    }

    debug!("Converting nodes");

    for (key, val) in gateway_format.local_nodes.iter() {

        debug!("Converting node {}", key);

        ds.local_nodes.push(val.clone());
    }

    debug!("Serializing data store");

    let serialized_data_store = match serde_json::to_string_pretty(&ds) {

        Ok(serialized) => {
            serialized
        },
        Err(or) => {
            warn!("Unable to serialize data store : {}", or);
            return Err("Unable to serialize data store");
        }
    };

    debug!("Writing file : {}", file_location);

    match common::write_to_file(&serialized_data_store, &file_location){
        Ok(_) => {
            debug!("Data written");
        },
        Err(or) => {
            warn!("Error putting data in file : {}", or);
            return Err(or);
        }
    }

    Ok(())
}

// ---------------------------------------------------------------------------
/// Load datastore and convert to the gateway format
// ---------------------------------------------------------------------------

pub fn gateway_load_and_convert(file: String) -> Result<GatewayFormat, &'static str> {

    debug!("Loading data from file");

    let data = match common::from_file(&file) {
        Some(d) => {
            d
        },
        None => {
            warn!("Error reading data from {}", file);
            return Err("Unable to load data from file ");
        }
    };

    debug!("Deserializing data");

    let deserialized_data_store : GatewayDataStore = match serde_json::from_str(&data) {
        Ok(data_store) => {
            data_store
        },
        Err(or) => {
            warn!("Unable to deserialize data store from {} : {}", file, or);
            return Err("Error deserializing data store");
        }
    };

    debug!("Converting to gateway format");

    let mut gateway_format : GatewayFormat = GatewayFormat {
        id:              deserialized_data_store.id,
        remote_gateways: HashMap::new(),
        local_nodes:     HashMap::new()
    };

    for item in deserialized_data_store.local_nodes.iter() {
        gateway_format.local_nodes.insert(item.id.clone(), item.clone());
    }

    for item in deserialized_data_store.remote_gateways.iter() {
        gateway_format.remote_gateways.insert(item.id.clone(), item.clone());
    }

    debug!("Completed");

    Ok(gateway_format)
}