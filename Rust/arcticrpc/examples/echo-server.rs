#[macro_use]
extern crate log;
extern crate simple_logger;
use log::Level;

extern crate arcticrpc;
extern crate serde_json;

fn echo(val: serde_json::Value) -> serde_json::Result<(serde_json::Value)> {

    Ok(val)
}

//  Main
//
pub fn main() {

    simple_logger::init_with_level(Level::Debug).unwrap();

    debug!("Register echo...");

    // Register methods
    arcticrpc::register(String::from("echo"), echo).unwrap();

    // Serve
    arcticrpc::serve("127.0.0.1:3031".to_string()).unwrap();
}