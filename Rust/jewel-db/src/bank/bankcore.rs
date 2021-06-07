/*

    Banked record storage on disk meant to serve as a simple database with a low in-memory footprint.

*/

use std::fs::File;
use std::fs::metadata;
use std::path::Path;
use std::io::prelude::*;

extern crate file_lock;
use file_lock::FileLock;

use crate::bank::bankerror::BankError;

use std::time;
use std::thread;
use std::fs;

/// The data bank structure
pub struct BankCore <'a> {

    pub bank_root: &'a Path,
    lock_attempts: u64,
    lock_wait:     time::Duration,
}

/// Create a new data bank
pub fn new_data_bank(bank_root: &str) -> Result<BankCore, BankError > {

    let md = match metadata(bank_root){
        Ok(md) => {
            md
        },
        Err(_) => {
            return Err(BankError::PathError("Unable to retrieve bank meta data"));
        }
    };

    if !md.is_dir() {
        return Err(BankError::PathError("Given path was not a directory"));
    }

    if md.permissions().readonly() {
        return Err(BankError::PathError("Jewel does not have write permissions for given path"));
    }

    Ok(BankCore{
        bank_root:      Path::new(bank_root),
        lock_attempts:  20,
        lock_wait:      time::Duration::from_millis(20)
    })
}

impl <'a> BankCore <'a> {

    /// Set the number of blocking lock attempts
    pub fn set_lock_attempts(&mut self, attempt: &u64) {
        self.lock_attempts = *attempt;
    }

    /// Set duration of time to wait between each lock attempt
    pub fn set_lock_wait_time(&mut self, wait_time: &time::Duration) {
        self.lock_wait = *wait_time;
    }

    /// Get a record
    pub fn get(&self, key: &str) -> Result< Vec<u8>, BankError > {

        let record = self.bank_root.join(Path::new(key));

        if !record.exists() {
            return Err(BankError::RecordNotFound("Key not found"));
        }

        let record_str = match record.to_str() {
            Some(rs) => { rs },
            None     => { return Err(BankError::InvalidKey) }  
        };

        for _ in 0..self.lock_attempts {
            match FileLock::lock(&record_str, true, false) {
                Ok(mut lock) => {

                    let mut data: Vec<u8> = Vec::new();

                    if let Err(_) = lock.file.read_to_end(&mut data){
                        return Err(BankError::RecordReadError) 
                    }

                    return Ok(data)
                },
                Err(_) => {
                    thread::sleep(self.lock_wait);
                    continue;
                },
            };
        }
        
        return Err(BankError::RecordReadError)
    }

    /// Set a record
    pub fn set(&self, key: &str, value: &Vec<u8>) -> Result<(), BankError >  {

        let record = self.bank_root.join(Path::new(key));

        let record_str = match record.to_str() {
            Some(rs) => { rs },
            None     => { return Err(BankError::InvalidKey) }  
        };

        if !record.exists() {
            // Create the record
            {
                if let Err(_) = File::create(&record) {
                    return Err(BankError::RecordCreationError) 
                }
            }
        } 

        for _ in 0..self.lock_attempts {
            match FileLock::lock(&record_str, true, true) {
                Ok(mut lock) => {

                    if let Err(_) = lock.file.write_all(value){
                        return Err(BankError::RecordWriteError) 
                    }

                    return Ok(())
                },
                Err(_) => {
                    thread::sleep(self.lock_wait);
                    continue;
                },
            };
        }
        
        return Err(BankError::RecordWriteError)
    }

    /// Delete a record
    pub fn remove(&self, key: &str) -> Result< (), BankError >  {

        let record = self.bank_root.join(Path::new(key));

        if !record.exists() {
            return Err(BankError::RecordNotFound("Key not found"));
        }

        let record_str = match record.to_str() {
            Some(rs) => { rs },
            None     => { return Err(BankError::InvalidKey) }  
        };

        for _ in 0..self.lock_attempts {
            match FileLock::lock(&record_str, true, false) {
                Ok(_lock) => {
                    match fs::remove_file(&record_str) {
                        Ok(_) => {
                            return Ok(())
                        },
                        Err(_) => {
                            return Err(BankError::RecordRemoveError)
                        }
                    }
                },
                Err(_) => {
                    thread::sleep(self.lock_wait);
                    continue;
                },
            };
        }

        Ok(())
    }
}

// -------------------------------------------[Tests]-------------------------------------------

#[cfg(test)]
mod tests {

    use crate::bank;
    use std::fs;
    use std::fs::metadata;
    use std::time;

    fn ensure_test_directory(path: &'static str) -> &'static str {

        if let Err(e) = fs::create_dir_all(&path) {

            let md = match metadata(&path){
                Ok(md) => {
                    md
                },
                Err(_) => {
                    panic!("Failed to create test bank, and failed to retrieve meta data");
                }
            };

            if !md.is_dir() {
                panic!("Failed to create test bank: {}", e);
            }
        }

        path
    }

    struct TestCase <'a> {
        key:    &'a str,
        value:  Vec<u8>
    }

    #[test]
    fn basic_bank_test(){
        let test_bank_path = ensure_test_directory("test/bank-0");

        let mut test_vec : Vec<TestCase> = Vec::new();

        test_vec.push(TestCase { key: "value_0", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_1", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_2", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_3", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_4", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });

        let new_bank = bank::new_data_bank(&test_bank_path).unwrap();

        for item in test_vec.iter() {
            if let Err(e) = new_bank.set(item.key, &item.value) {
                panic!("Failed to create new record : {}", e);
            }
        }

        for item in test_vec.iter() {
            let value = match new_bank.get(item.key) {
                Ok(v) => {
                    v
                },
                Err(e) => {
                    panic!("Error getting known item : {}", e);
                }
            }; 

            println!("value: {:?} | read-in: {:?}", value, item.value);

            assert!(value == item.value);
        }
    }

    #[test]
    fn remove_test(){
        let test_bank_path = ensure_test_directory("test/bank-1");

        let mut test_vec : Vec<TestCase> = Vec::new();

        test_vec.push(TestCase { key: "value_0", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_1", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_2", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_3", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_4", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_5", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_6", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_7", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_8", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });

        let new_bank = bank::new_data_bank(&test_bank_path).unwrap();

        for item in test_vec.iter() {
            if let Err(e) = new_bank.set(item.key, &item.value) {
                panic!("Failed to create new record : {}", e);
            }
        }

        if let Err(e) = new_bank.remove("value_4") {
            panic!("Unable to remove item : {}", e);
        }

        if let Err(e) = new_bank.remove("value_7") {
            panic!("Unable to remove item : {}", e);
        }

        match new_bank.get("value_4") {
            Ok(_) => {
                panic!("Retrieved item that was suppose to have been removed");
            },
            Err(_) => { }
        }
        match new_bank.get("value_2") {
            Ok(_) => { },
            Err(e) => {
                panic!("Unable to retrieve known-good value while testing removal : {}", e);
            }
        }
        
        match new_bank.get("value_7") {
            Ok(_) => {
                panic!("Retrieved item that was suppose to have been removed");
            },
            Err(_) => { }
        }
    }

    #[test]
    fn custom_locking_test() {

        let test_bank_path = ensure_test_directory("test/bank-2");

        let mut new_bank = bank::new_data_bank(&test_bank_path).unwrap();
        new_bank.set_lock_attempts(&1);
        new_bank.set_lock_wait_time(&time::Duration::from_secs(1));

        let mut test_vec : Vec<TestCase> = Vec::new();
        test_vec.push(TestCase { key: "value_0", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_1", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });
        test_vec.push(TestCase { key: "value_2", value: (0..255).map(|_| { rand::random::<u8>() }).collect() });

        for item in test_vec.iter() {
            if let Err(e) = new_bank.set(item.key, &item.value) {
                panic!("Failed to create new record : {}", e);
            }
        }
    }

    #[test]
    #[should_panic]
    fn custom_bad_locking_test() {

        let test_bank_path = ensure_test_directory("test/bank-3");
        let mut new_bank = bank::new_data_bank(&test_bank_path).unwrap();

        // This lock attempt count should cause an error
        new_bank.set_lock_attempts(&0);
        new_bank.set_lock_wait_time(&time::Duration::from_secs(10000)); // This should do nothing b/c of the previous line

        if let Err(e) = new_bank.set("Some-key-that-wont-work", &(0..255).map(|_| { rand::random::<u8>() }).collect()) {
            panic!("Failed to create new record : {}", e);
        }
    }

}