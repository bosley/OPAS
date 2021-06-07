//extern crate log;
//extern crate simple_logger;
//use log::Level;

extern crate arcticrpc;
extern crate serde_json;
use serde_json::json;

//  Main
//
pub fn main() {

    //simple_logger::init_with_level(Level::Debug).unwrap();

    let result =  match arcticrpc::call(
        &String::from("127.0.0.1:3030"), 
        String::from("add"), 
        json!({ "lhs": 42, "rhs": 8 })
    ){
        Ok(n) => n,
        Err(e) => panic!("Error calling server {:?}", e)
    };

    println!("Client got back data : {}", result);
    assert!(result == 50);
}