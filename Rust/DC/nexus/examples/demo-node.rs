extern crate log;
extern crate simple_logger;
use log::Level;
use std::env;

extern crate nexus;
//
//  Entry
//
fn main() {
    simple_logger::init_with_level(Level::Debug).unwrap();

    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        eprintln!("Usage: {} <node file>", &args[0]);
        return;
    }

    println!("Loading node file [{}]", &args[1]);

    let mut node = match nexus::node::load_node(&args[1]) {
        Ok(n) => {
            n
        },
        Err(e) => {
            eprintln!("Error: {}", e);
            return;
        }
    };

    match node.start(10) {
        Ok(_) => {
            println!("Node started!");
        },
        Err(e) => {
            eprint!("Error starting node : {}", e);
            return;
        }
    }
}
