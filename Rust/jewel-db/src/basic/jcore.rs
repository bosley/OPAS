
use crate::basic::settings;
use crate::basic::DbError;
use crate::basic::record;
use crate::basic;

use std::sync::Mutex;
use std::collections::HashSet;

/// Load a database given a settings object
pub fn load_database(settings: &settings::Settings) -> Result< JewelCore, DbError > {

    // Load data if it exists, or create a new datastore
    let stored_data = match helpers::from_file(&settings.data_store){
        Some(data) => {
            data
        },
        None => {
            // File doesn't exist
            return Ok(JewelCore{
                db_path:    settings.data_store.clone(),
                data_set:   Mutex::new(HashSet::new())
            });
        }
    };

    // Take stored data and convert it into the vector that it represents
    let vec_data : Vec<record::JewelRecord> = match serde_json::from_str(&stored_data) {
        Ok(data) => {
            data
        },
        Err(_) => {
            return Err(DbError{
                code:       basic::dberror::DB_DESERIALIZATION,
                message:    String::from("Deserialization issue")
            });
        }
    };

    // Create a core with the vector data represented as a hash set
    Ok(JewelCore{
        db_path:    settings.data_store.clone(),
        data_set:   Mutex::new(
            helpers::vec_to_set(vec_data)
        )
    })
}

//
/// Core object for the jewel database
//
pub struct JewelCore {

    db_path: String,
    data_set: Mutex<HashSet<record::JewelRecord>>
}

impl JewelCore {

    /// Get the name of the current database being used
    pub fn current_db(&self) -> String {
        self.db_path.clone()
    }

    /// Set a value
    pub fn set(&mut self, key: &String, value: &Vec<u8>) {

        let record = record::new(key, value);

        let mut set_guard = match self.data_set.lock() {
            Ok(guard)       => guard,
            Err(poisoned)   => poisoned.into_inner(),
        };

        set_guard.insert(record);
    }

    /// Get a value
    pub fn get(&self, key: &String) -> Result< Vec<u8>, DbError > {

        let set_guard = match self.data_set.lock() {
            Ok(guard)       => guard,
            Err(poisoned)   => poisoned.into_inner(),
        };

        match set_guard.get(&record::from(key)) {

            Some(record) => {
                Ok(record.get_value())
            },

            None => {
                return Err(DbError{
                    code:       basic::dberror::RECORD_NOT_FOUND,
                    message:    String::from("Record not found during 'get'")
                });
            }
        }
    }

    /// Remove a value
    pub fn remove(&mut self, key: &String) -> Result<(), DbError> {

        let mut set_guard = match self.data_set.lock() {
            Ok(guard)       => guard,
            Err(poisoned)   => poisoned.into_inner(),
        };

        if !set_guard.remove(&record::from(key)) {
            return Err(DbError{
                code:       basic::dberror::RECORD_NOT_FOUND,
                message:    String::from("Record not found during 'remove'")
            });
        }

        Ok(())
    }

    pub fn full_save(&self) -> Result<(), DbError> {

        let set_guard = match self.data_set.lock() {
            Ok(guard)       => guard,
            Err(poisoned)   => poisoned.into_inner(),
        };

        let vec_to_save = match serde_json::to_string(
            &helpers::set_to_vec(&set_guard.clone())
        ){
            Ok(vec_string) => {
                vec_string
            },
            Err(_) => {
                return Err(DbError{
                    code:       basic::dberror::DB_SERIALIZATION,
                    message:    String::from("Unable to serialize data during 'full_save'")
                });
            }
        };

        match helpers::write_to_file(&vec_to_save, &self.db_path) {
            Ok(_) => {
                return Ok(())
            },
            Err(e) => {
                return Err(DbError{
                    code:       basic::dberror::DB_WRITE_FAILURE,
                    message:    String::from(e)
                });
            }
        }
    }
}

mod helpers {

    use std::fs::File;
    use std::path::Path;
    use std::io::{Read, Write};
    use std::collections::HashSet;
    use crate::basic::record;
    use std::iter::FromIterator;

    pub fn set_to_vec(set: &HashSet<record::JewelRecord>) -> Vec<record::JewelRecord> {
        set.iter().cloned().collect::<Vec<record::JewelRecord>>().into()
    }

    pub fn vec_to_set(vec: Vec<record::JewelRecord>) -> HashSet<record::JewelRecord> {
        HashSet::from_iter(vec.iter().cloned())
    }

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
                return None;
            }
        };

        let mut buff = String::new();
        if let Err(_) = o.read_to_string(&mut buff){
            return None;
        };
        drop(o);

        Some(buff)
    }
}

// -------------------------------------------[Tests]-------------------------------------------

#[cfg(test)]
mod tests {

    use crate::basic;

    #[derive(Debug, Clone)]
    struct TestData{
        key:    String,
        value:  Vec<u8>
    }

    fn new_test_data(key: String, value: Vec<u8>) -> TestData{
        TestData{
            key:    key.clone(),
            value:  value.clone()
        }
    }

    #[test]
    fn test_default() {

        let default_settings = basic::Settings::default();
        let jewel_core       = match basic::load_database(&default_settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        assert_eq!(String::from(".jewel.db"), jewel_core.current_db());
    }

    #[test]
    fn test_set_get() {

        let default_settings = basic::Settings::default();
        let mut jewel_core       = match basic::load_database(&default_settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        let test_values : Vec<TestData> = [
            new_test_data("value_0".to_string(), [ 45,   45,  90].to_vec()),
            new_test_data("value_1".to_string(), [ 31,   41,  59].to_vec()),
            new_test_data("value_2".to_string(), [ 21,   82,  80].to_vec()),
            new_test_data("value_3".to_string(), [100,   40,  40].to_vec()),
            new_test_data("value_4".to_string(), [255,  255,  90].to_vec()),
        ].to_vec();

        for test_data in test_values.iter() {
            jewel_core.set(&test_data.key, &test_data.value);
        }

        for test_data in test_values.iter() {

             match jewel_core.get(&test_data.key) {
                Ok(retrieved_val) => {
                    if retrieved_val != test_data.value {
                        panic!("Got incorrect value for given key : Expected [{:?}] got [{:?}]", test_data.value, retrieved_val);
                    }
                },
                Err(e) => {
                    panic!("Unable to get known value from key : {}", e);
                }
            };
        }
    }

    #[test]
    fn test_remove() {
        let default_settings = basic::Settings::default();
        let mut jewel_core   = match basic::load_database(&default_settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        let test_values : Vec<TestData> = [
            new_test_data("value_0".to_string(), [ 45,   45,  90].to_vec()),
            new_test_data("value_1".to_string(), [ 31,   41,  59].to_vec()),
            new_test_data("value_2".to_string(), [ 21,   82,  80].to_vec()),
            new_test_data("value_3".to_string(), [100,   40,  40].to_vec()),
            new_test_data("value_4".to_string(), [255,  255,  90].to_vec()),
        ].to_vec();

        for test_data in test_values.iter() {
            jewel_core.set(&test_data.key, &test_data.value);
        }

        if let Err(e) = jewel_core.remove(&"value_2".to_string()) {
            panic!("Unable to delete key known to exist : {}", e);
        }

        match jewel_core.get(&"value_2".to_string()) {
            Ok(_) => {
                panic!("Was able to retrieve a deleted key");
            },
            Err(_) => {
                // Expected
            }
        }
    }

    #[test]
    fn test_save() {

        let default_settings = basic::Settings::default();
        let mut jewel_core   = match basic::load_database(&default_settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        let test_values : Vec<TestData> = [
            new_test_data("value_0".to_string(), [ 45,   45,  90].to_vec()),
            new_test_data("value_1".to_string(), [ 31,   41,  59].to_vec()),
            new_test_data("value_2".to_string(), [ 21,   82,  80].to_vec()),
            new_test_data("value_3".to_string(), [100,   40,  40].to_vec()),
            new_test_data("value_4".to_string(), [255,  255,  90].to_vec()),
        ].to_vec();

        for test_data in test_values.iter() {
            jewel_core.set(&test_data.key, &test_data.value);
        }

        if let Err(e) = jewel_core.full_save() {
            panic!("Error saving core: {}", e);
        }
    }

    #[test]
    fn test_load() {
        
        let mut settings = basic::Settings::default();
        settings.data_store = "save-load-test.db".to_string();

        let mut jewel_core   = match basic::load_database(&settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        let test_values : Vec<TestData> = [
            new_test_data("value_0".to_string(), [ 33,   24,  33].to_vec()),
            new_test_data("value_1".to_string(), [ 24,   33,  24].to_vec()),
            new_test_data("value_2".to_string(), [ 33,   24,  33].to_vec()),
            new_test_data("value_3".to_string(), [ 45,   45,  90].to_vec()),
            new_test_data("value_4".to_string(), [ 31,   41,  59].to_vec()),
            new_test_data("value_5".to_string(), [ 21,   82,  80].to_vec()),
            new_test_data("value_6".to_string(), [100,   40,  40].to_vec()),
            new_test_data("value_7".to_string(), [255,  255,  90].to_vec()),
            new_test_data("value_8".to_string(), [100,   40,  40].to_vec()),
            new_test_data("value_9".to_string(), [255,  255,  90].to_vec()),
        ].to_vec();

        for test_data in test_values.iter() {
            jewel_core.set(&test_data.key, &test_data.value);
        }

        if let Err(e) = jewel_core.full_save() {
            panic!("Error saving core: {}", e);
        }

        let loaded_core = match basic::load_database(&settings){
            Ok(jc) => { jc },
            Err(e) => { panic!("{}", e); }
        };

        for test_data in test_values.iter() {

            let data = match loaded_core.get(&test_data.key) {
                Ok(d) => {
                    d
                },
                Err(_) => {
                    panic!("Failed to retrieve stored data");
                }
            };

            assert!(data == test_data.value);
        }
    }
}



