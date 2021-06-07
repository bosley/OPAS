extern crate lazy_static;

extern crate serde;
use serde::{Deserialize, Serialize};
extern crate serde_json;

use std::sync::Mutex;
use std::collections::HashMap;
use std::time::Duration;

//
//  Constants used for data transmission and error detection
//
pub const NET_RESP_VALID:   [u8; 4]  = [1, 1, 1, 1]; // 'Good' Response
pub const NET_RESP_INVALID: [u8; 4]  = [0, 0, 0, 1]; // 'Bad'  Response
pub const NET_LOCK_FAILED:  [u8; 4]  = [0, 0, 0, 2]; // Lock failure on exec
pub const NET_NO_METHOD:    [u8; 4]  = [0, 0, 0, 3]; // No method by name called
pub const NET_DATA_LEN:     usize    = 8;  // Max data blocks to expect on a new connection (Limits tx size to 65535 bytes)

pub const THREAD_LIMIT:     usize    = 10;    
pub const READ_TIMEOUT:     Duration = Duration::from_secs(10);

//
///  Function type required for a 'Callable' method
//
pub type Callable = fn(serde_json::Value) -> serde_json::Result<(serde_json::Value)>;

//
//  Structure to send containing data required to 
//  call a remote method
//
#[derive(Clone, Serialize, Deserialize)]
pub struct Frame {
    pub called_method: String,
    pub called_params: serde_json::Value
}

//
//  Structure to send resulting information back from a call
//
#[derive(Clone, Serialize, Deserialize)]
pub struct FrameResult {
    pub result_value: serde_json::Value
}

//
//  Map function names to methods
//
lazy_static! {
    pub static ref FUCNTION_MAP: Mutex<HashMap<String, Callable>> = Mutex::new(HashMap::new());
}

//
//  Register a method to be callable from a client
//
pub fn register(method_name: String, method: Callable) -> Result< bool, &'static str> {

    //! Register a Callable method with the server identified by the String 'method_name.'
    //! When a client makes a request to execute 'method_name' the given function will be 
    //! executed and data returned provided the method executes without fail

    let mut c = FUCNTION_MAP.lock().map_err(|_| "Failed to acquire MutexGuard")?;
    c.insert(method_name, method);
    Ok(true)
}

//
//  Execute a method
//
pub fn execute(frame: Frame) -> Vec<u8> {

    let method = {
        match FUCNTION_MAP.lock() {
            Ok(l) => l,
            Err(_) => return NET_LOCK_FAILED.to_vec()
        }
    };

    if !method.contains_key(&frame.called_method){
        return NET_NO_METHOD.to_vec();
    }

    match method[&frame.called_method](frame.called_params) {

        Ok(n) => {

            let frame_response = FrameResult {
                result_value: n
            };


            let json_value =  match serde_json::to_string(&frame_response) {
                Ok(n) => n,
                Err(_) => return Vec::new()
            };

            return bincode::serialize(&json_value).unwrap()
        }

        Err(_) => {

            let frame_response = FrameResult {
                result_value: serde_json::Value::from("Error calling method")
            };

            let json_value =  match serde_json::to_string(&frame_response) {
                Ok(n) => n,
                Err(_) => return Vec::new()
            };

            return bincode::serialize(&json_value).unwrap()
        }
    };
}