extern crate log;
extern crate simple_logger;
use log::Level;

extern crate arcticrpc;

use std::{thread, time};
extern crate serde_json;

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

//  Main
//
pub fn main() {

    simple_logger::init_with_level(Level::Debug).unwrap();

    // Register methods
    arcticrpc::register(String::from("add"), add).unwrap();

    println!("Starting non-blocking server");

    // Start server
    arcticrpc::serve_threaded("127.0.0.1:3030".to_string());

    println!(">> Server will operate for 10 seconds before shutting down...");

    let seconds_wait = time::Duration::from_secs(10);

    thread::sleep(seconds_wait);

    println!("Stopping non-blocking server");

    // Stop server
    arcticrpc::stop();
}