use std::hash::{Hash, Hasher};

use serde::{Serialize, Deserialize};

//
/// A data record for the jewel db
//
#[derive(Clone, Debug, Serialize, Deserialize, Eq)]
pub struct JewelRecord {

    key:            String,
    value:          Vec<u8>
}

//
/// Create a new record from a key and value
//
pub fn new(key: &String, value: &Vec<u8>) -> JewelRecord{
    JewelRecord{
        key:        key.clone(),
        value:      value.clone()
    }
}

pub fn from(key: &String) -> JewelRecord{
    JewelRecord{
        key:        key.clone(),
        value:      Vec::new()
    }
}

//
/// Jewel Record Implementations
//
impl JewelRecord {

    /// Get the key
    pub fn get_key(&self) -> String {
        self.key.clone()
    }

    /// Get the value
    pub fn get_value(&self) -> Vec<u8> {
        self.value.clone()
    }

    /// Set the value
    pub fn set_value(&mut self, value: &Vec<u8>) {
        self.value = value.clone();
    }
}

// 
/// Compare string aginst record checks record key for equality
//
impl PartialEq<String> for JewelRecord {
    fn eq(&self, other: &String) -> bool {
        &self.key == other
    }
}

//
/// Compare record against record checks for key, not key/val
//
impl PartialEq<JewelRecord> for JewelRecord {
    fn eq(&self, other: &JewelRecord) -> bool {
         &self.key   == &other.key
    }
}

//
/// Compare Vector against record checks record value for equality
//
impl PartialEq<Vec<u8>> for JewelRecord {
    fn eq(&self, other: &Vec<u8>) -> bool {
        &self.value == other
    }
}

//
/// Hashing record hashes key, forcing keys to be unique
//
impl Hash for JewelRecord {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.key.hash(state);
    }
}

impl From<String> for JewelRecord {
    fn from(s: String) -> JewelRecord {
        new(&s, &Vec::new())
    }
}


#[cfg(test)]
mod tests {

    use crate::basic;

    #[test]
    fn test_eq_key(){

        let a = basic::record::new(&"key_1".to_string(), &[45, 45, 45, 0, 222].to_vec());

        assert!(a == "key_1".to_string());
    }

    #[test]
    fn test_eq_val(){

        let a = basic::record::new(&"key_1".to_string(), &[45, 45, 45, 0, 222].to_vec());

        assert!(a == [45, 45, 45, 0, 222].to_vec());
    }

    #[test]
    fn test_eq_record(){

        let a = basic::record::new(&"key_1".to_string(), &[222, 55, 44, 90, 55].to_vec());
        let b = basic::record::new(&"key_1".to_string(), &[222, 55, 44, 90, 55].to_vec());

        assert!(a == a);
        assert!(b == b);
        assert!(a == b);

        let c = basic::record::new(&"key_2".to_string(), &[222, 55, 44, 90, 55].to_vec());
        let d = basic::record::new(&"key_3".to_string(), &[222, 66, 66, 90, 55].to_vec());

        assert!(!(c == b));
        assert!(!(c == d));
        assert!(!(d == a));
    }

    #[test]
    fn test_serialization(){

        let a = basic::record::new(&"key_1".to_string(), &[222, 55, 44, 90, 55].to_vec());

        let a_ser = match serde_json::to_string_pretty(&a) {
            Ok(ser) => {
                ser
            }, 
            Err(e) => {
                panic!("Error serializing record : {}", e);
            }
        };

        let a_deser : basic::record::JewelRecord = match serde_json::from_str(&a_ser) {
            Ok(deser) => {
                deser
            },
            Err(e) => {
                panic!("Error deserializing serialized record : {}", e);
            }
        };

        assert!(a == a_deser);
    }
}