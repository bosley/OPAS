
extern crate log;
extern crate simple_logger;
use log::Level;

extern crate nexus;

use std::thread;
use std::time;

extern crate ctrlc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;


extern crate clap;
use clap::{Arg, App};

fn main() {
    let matches = App::new("Nexus Service Application")
                    .version("1.0")
                    .author("Josh A. Bosley <bosley117@gmail.com>")
                    .about("Launch a demo")

                        .arg(Arg::with_name("app")
                            .long("ip")
                            .help("Sets the ip")
                            .required(true)
                            .takes_value(true))

                        .arg(Arg::with_name("port")
                            .long("port")
                            .help("Sets the port")
                            .required(true)
                            .takes_value(true))

                        .arg(Arg::with_name("name")
                            .long("name")
                            .help("Sets the name")
                            .required(true)
                            .takes_value(true))

                        .get_matches();
    /*
    
            Setup
    
    */
    simple_logger::init_with_level(Level::Debug).unwrap();

    // Start the node and receive a node controller object
    let mut node_controller = match nexus::node::start(
        matches.value_of("ip").unwrap().to_string(),
        matches.value_of("port").unwrap().to_string(),
        matches.value_of("name").unwrap().to_string()){
            Ok(node_thread_interface) => {
                node_thread_interface
            },
            Err(e) => {
                panic!("Node got an error while starting node thread : {}", e);
            }
        };

    // Setup CTRL-C
    let running = Arc::new(AtomicBool::new(true));
    let r = running.clone();

    ctrlc::set_handler(move || {
        r.store(false, Ordering::SeqCst);
    }).expect("Error setting Ctrl-C handler");

    println!("Use Ctrl-C to exit");

    /*
    
            Run the app until CTRL-C Given
    
    */
    while running.load(Ordering::SeqCst) {
        


        let application_messages : Vec<nexus::node::NodeMessage> = node_controller.data_receiver.try_iter().collect();

        for message in application_messages {

            println!("Got application message from {}", message.connection);
        }

        thread::sleep(time::Duration::from_millis(500));
    }

    println!("");

    /*
    
            Command the node thread to die
    
    */
    if let Err(e) = node_controller.command_sender.send(
        nexus::node::NodeCommand::StopThread
    ){
        panic!("Unable to send STOP command to thread :: {}", e);
    }

    // Join the given thread handle for the node
    node_controller.thread_handle
        .take().expect("Called stop on non-running thread")
            .join().expect("Could not join command service thread");
}