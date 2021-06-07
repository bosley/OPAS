extern crate arcticrpc;
extern crate serde_json;
use serde_json::json;

fn get_req() -> serde_json::Value {
    json!({ "data": "Some dummy data",
                       "answer":  42,
                       "pi": 3.14159})
}

//  Main
//
pub fn main() {

    let result =  match arcticrpc::call(
        &String::from("127.0.0.1:3031"), 
        String::from("echo"), 
        get_req()
    ){
        Ok(n) => n,
        Err(e) => panic!("Error calling server {:?}", e)
    };

    assert!(result == get_req());

    println!("Success!");
}