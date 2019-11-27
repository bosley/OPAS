use std::collections::HashMap;
use std::sync::{Arc, Mutex};

extern crate serde;
use serde::{Deserialize, Serialize};

pub struct DataStore {
    file:     String,
    pub ds:   Arc<Mutex<Storage>>
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Storage {
    network: HashMap<String, String>
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct NetworkObject {
    pub key: String,
    pub value: String
}

pub fn new_data_store(file: &String) -> DataStore {

    DataStore{
        file: file.to_string(),
        ds:   Arc::new(Mutex::new(Storage{
            network: HashMap::new()
        }))
    }
}

/// Load a data store
pub fn get_data_store(file: &String) -> Result<DataStore, &'static str> {

    let storage_data = match helpers::from_file(file) {
        Some(data) => {
            data
        },
        None => {
            return Err("Unable to read from file");
        }
    };

    let deserialized_data = match serde_json::from_str(&storage_data) {
        Ok(data) => {
            data
        },
        Err(_) => {
            return Err("Error deserializing data from file");
        }
    };

    Ok(DataStore{
        file: file.to_string(),
        ds:   Arc::new(Mutex::new(Storage{
            network: deserialized_data
        }))
    })
}

impl DataStore {

    /// Write data store
    pub fn write(&self) -> Result<(), &'static str> {

        let ds_guard    = match self.ds.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let serialized_ds = match serde_json::to_string_pretty(&ds_guard.network) {
            Ok(data) => {
                data
            },
            Err(e) => {
                debug!("Serialization Error {}", e);
                return Err("Error serializing data");
            }
        };

        drop(ds_guard);

        match helpers::write_to_file(&serialized_ds, &self.file) {
            Ok(_) => {
                Ok(())
            },
            Err(e) => {
                return Err(e);
            }
        }
    }

    #[allow(dead_code)]
    pub fn reload(&mut self) -> Result<(), &'static str> {

        let storage_data = match helpers::from_file(&self.file) {
            Some(data) => {
                data
            },
            None => {
                return Err("Unable to read from file");
            }
        };
    
        let deserialized_data = match serde_json::from_str(&storage_data) {
            Ok(data) => {
                data
            },
            Err(_) => {
                return Err("Error deserializing data from file");
            }
        };

        let mut ds_guard    = match self.ds.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        ds_guard.network = deserialized_data;

        Ok(())
    }

    pub fn insert(&mut self, id: &String, connection: String) {

        let mut ds_guard    = match self.ds.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        ds_guard.network.insert(id.to_string(), connection);
    }

    #[allow(dead_code)]
    pub fn get(&self, id: &String) -> Option<String> {

        let ds_guard    = match self.ds.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        match ds_guard.network.get(id) {
            Some(data) => {
                Some(data.to_string())
            },
            None => {
                None
            }
        }
    }

    pub fn retrieve_all_network_objects(&self) -> Vec<NetworkObject> {

        let ds_guard    = match self.ds.lock() {
            Ok(guard)     => guard,
            Err(poisoned) => poisoned.into_inner(),
        };

        let mut net_obj_list : Vec<NetworkObject> = Vec::new();

        for (key, value) in ds_guard.network.iter() {
            net_obj_list.push(
                NetworkObject{
                    key:   key.to_string(),
                    value: value.to_string()
                }
            );
        }

        net_obj_list
    }
}

mod helpers {

    use std::fs::File;
    use std::path::Path;
    use std::io::{Read, Write};

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
}

#[cfg(test)]
mod test {

    use crate::datastore;

    // Create a new datastore, load it into another. Add node into loaded node, and write out
    // reload the first datastore and ensure that new node exists
    #[test]
    fn create_new_ds() {
        let mut new_ds = datastore::new_data_store(&"test_ds.json".to_string());

        for x in 0..20 {
            new_ds.insert(
                &format!("node-{}",x).to_string(), format!("192.168.1.{}", x+1).to_string()
            );
        }

        if let Err(e) =  new_ds.write() {
            panic!("Error writing datastore: {}", e);
        }

        let mut other_ds = match datastore::get_data_store(&"test_ds.json".to_string()) {
            Ok(ds) => {
                ds
            },
            Err(e) => {
                panic!("Unable to load ds {}", e);
            }
        };

        other_ds.insert(&"demo-node".to_string(), "0.0.0.0:9090".to_string());

        if let Err(e) =  other_ds.write() {
            panic!("Error writing datastore: {}", e);
        }

        if let Err(e) = new_ds.reload() {
            panic!("Unable to reload datastore : {}", e);
        }

        if let None = new_ds.get(&"demo-node".to_string()) {
            panic!("Unable to get new node ");
        }
    }
}