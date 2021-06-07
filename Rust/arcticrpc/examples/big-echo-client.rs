extern crate arcticrpc;
extern crate serde_json;
use serde_json::json;

extern crate rand;
use rand::prelude::*;

//  Main
//
pub fn main() {
    
    let mut rng = rand::thread_rng();
    let numbers: Vec<i64> = (0..1000000).map(|_| {  // Largest possible: 2147483647 (i32 MAX)
        rng.gen_range(0, 10)
    }).collect();

    let data = json!({ "data": Vec::clone(&numbers) });

    let result =  match arcticrpc::call(
        &String::from("127.0.0.1:3031"), 
        String::from("echo"), 
        data
    ){
        Ok(n) => n,
        Err(e) => panic!("Error calling server {:?}", e)
    };

    let result_data = json!({ "data": Vec::clone(&numbers) });

    assert!(result == result_data);

    println!("Success!");
}