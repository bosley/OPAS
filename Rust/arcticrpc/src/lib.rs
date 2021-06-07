// -- Framework
mod framework;
pub use framework::register as register;
pub use framework::Callable as Callable;

// -- Server
mod server;
pub use server::serve as serve;
pub use server::stop  as stop;
pub use server::serve_threaded as serve_threaded;

// -- Client
mod client;
pub use client::call as call;

// -- Macros used
#[macro_use]
extern crate lazy_static;

#[macro_use]
extern crate log;
extern crate simple_logger;

// ----------------------------------------- Tests -----------------------------------------

#[cfg(test)]
mod tests {

    use crate::framework;
    extern crate serde_json;
    use serde_json::json;

    use crate::server;
    use crate::client;

    // ----------------------------------- framework.rs ------------------------------------

    fn exec_func(_param: serde_json::Value) -> serde_json::Result<(serde_json::Value)> {
        Ok(json!({
            "response_field_0": "dummy",
            "response_field_1": 42
        }))
    }

    #[test]
    fn framework_call_frame() {

        let call_frame : framework::Frame = framework::Frame {
            called_method: String::from("some_method"),
            called_params: serde_json::Value::from("some_string_param")
        };

        let call_frame_json =  match serde_json::to_string(&call_frame) {
            Ok(n) => n,
            Err(_) => panic!("Could not run serde_json::to_string on framework::Frame")
        };

        let _encoded: Vec<u8> = bincode::serialize(&call_frame_json).unwrap();

        // --------------

        let call_frame : framework::Frame = framework::Frame {
            called_method: String::from("some_method"),
            called_params: json!({
                "param_0": 42,
                "param_1": "Some_string"
            })
        };

        let call_frame_json =  match serde_json::to_string(&call_frame) {
            Ok(n) => n,
            Err(_) => panic!("Could not run serde_json::to_string on framework::Frame")
        };

        let _encoded: Vec<u8> = bincode::serialize(&call_frame_json).unwrap();
    }

    #[test]
    fn framework_result_frame() {

        let frame_result : framework::FrameResult = framework::FrameResult {
            result_value: serde_json::Value::from("some_string_result")
        };

        let resp_frame_json =  match serde_json::to_string(&frame_result) {
            Ok(n) => n,
            Err(_) => panic!("Could not run serde_json::to_string on framework::FrameResult")
        };

        let _encoded: Vec<u8> = bincode::serialize(&resp_frame_json).unwrap();

        // --------------

        let frame_result : framework::FrameResult = framework::FrameResult {
            result_value: json!({
                "response_field_0": 42,
                "response_field_1": "Some_string",
                "response_field_2": "Some_other_string"
            })
        };

        let resp_frame_json =  match serde_json::to_string(&frame_result) {
            Ok(n) => n,
            Err(_) => panic!("Could not run serde_json::to_string on framework::FrameResult")
        };

        let _encoded: Vec<u8> = bincode::serialize(&resp_frame_json).unwrap();
    }

    #[test]
    fn framework_full_struct_tests() {

        framework::register(String::from("exec_func"), exec_func).unwrap();

        let exec_frame : framework::Frame = framework::Frame {
            called_method: String::from("exec_func"),
            called_params: json!({
                "param_0": 42,
                "param_1": "Some_string"
            })
        };

        let result : Vec<u8> = framework::execute(exec_frame);
        let bad    : Vec<u8>= Vec::new();

        assert!(result != bad);
        assert!(result != framework::NET_LOCK_FAILED.to_vec());
    }

    // ----------------------------------- client/server ------------------------------------
    
    fn add(val: serde_json::Value) -> serde_json::Result<(serde_json::Value)> {
        let number = val.as_object().unwrap();
        let lhs = number["lhs"].as_i64().unwrap();
        let rhs = number["rhs"].as_i64().unwrap();
        Ok(serde_json::Value::from(
            lhs + rhs
            ))
    }

    #[test]     
    #[ignore]   // Ignore incase bind fails on travis (intermittent)
    fn test_connect_good() {
        
        framework::register(String::from("add"), add).unwrap();
        server::serve_threaded("0.0.0.0:4296".to_string());

        let result =  match client::call(
            &String::from("0.0.0.0:4296"), 
            String::from("add"), 
            json!({ "lhs": 33, "rhs": 7 })
            ){
                Ok(n) => n,
                Err(e) => panic!("Error calling server {:?}", e)
            };

        assert!(result == 40);
        
        server::stop();
    }

    #[test]
    #[should_panic]
    fn test_connect_bad_result() {
        
        framework::register(String::from("add"), add).unwrap();
        server::serve_threaded("0.0.0.0:4295".to_string());

        let result =  match client::call(
            &String::from("0.0.0.0:4295"), 
            String::from("add"), 
            json!({ "lhs": 33, "rhs": 7 })
            ){
                Ok(n) => n,
                Err(e) => panic!("Error calling server {:?}", e)
            };

        assert!(result == 9000);
        
        server::stop();
    }
    
    #[test]
    #[should_panic]
    fn test_connect_bad_call() {
        
        framework::register(String::from("add"), add).unwrap();
        server::serve_threaded("0.0.0.0:4294".to_string());

        let result =  match client::call(
            &String::from("0.0.0.0:4294"), 
            String::from("something_that_doesn't_exist"), 
            json!({ "lhs": 33, "rhs": 7 })
            ){
                Ok(n) => n,
                Err(e) => panic!("Error calling server {:?}", e)
            };

        assert!(result == 9000);
        
        server::stop();
    }
}