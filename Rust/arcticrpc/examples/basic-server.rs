extern crate log;
extern crate simple_logger;
use log::Level;

extern crate arcticrpc;
extern crate serde_json;
use serde_json::json;

//  Value is passed into all callable methods. It is up-to author to 
//  ensure safety of fields within value and a correct return
//
fn add(val: serde_json::Value) -> serde_json::Result<(serde_json::Value)> {

    let number = val.as_object().unwrap();

    let lhs = number["lhs"].as_i64().unwrap();
    let rhs = number["rhs"].as_i64().unwrap();

    Ok(serde_json::Value::from(
        lhs + rhs
        ))
}

//  Test the local calls as an example of usage
//
fn local_calls(){
    // add
    //
    let result : serde_json::Value = match add(json!({
        "lhs": 42,
        "rhs": 8
    })) {
        Ok(n) => n,
        Err(_) => serde_json::Value::from("Error calling add locally")
    };

    assert!(result == 50);
}

//  Main
//
pub fn main() {

    simple_logger::init_with_level(Level::Debug).unwrap();

    // Register methods
    arcticrpc::register(String::from("add"), add).unwrap();

    // Test locally
    local_calls();

    // Serve
    arcticrpc::serve("127.0.0.1:3030".to_string()).unwrap();
}