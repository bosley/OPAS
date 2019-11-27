
extern crate log;
extern crate simple_logger;
use log::Level;

extern crate nexus;

use std::thread;
use std::time;

extern crate ctrlc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

fn main() {

    /*
    
            Setup
    
    */
    simple_logger::init_with_level(Level::Debug).unwrap();

    // Start the node and receive a node controller object
    let mut node_controller = match nexus::node::start(
        "127.0.0.1".to_string(),
        "4042".to_string(),
        "bob".to_string()){
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

        // Request that alice '127.0.0.1:4091' register bob (local) 
        node_controller.command_sender.send(
            nexus::node::NodeCommand::RequestRegistration(
                ("bob".to_string(), "127.0.0.1:4041".to_string())
        )).unwrap();

        // Direct send
        node_controller.command_sender.send(nexus::node::NodeCommand::SendData(
            nexus::node::NodeMessage{
                connection: "127.0.0.1:4041".to_string(),
                data:       [45, 45, 45, 45 ,10, 92].to_vec()
            }
        )).unwrap();

        // Broadcast send
        node_controller.command_sender.send(nexus::node::NodeCommand::Broadcast(
            [33, 33, 33, 33, 33].to_vec()
        )).unwrap();

        let application_messages : Vec<nexus::node::NodeMessage> = node_controller.data_receiver.try_iter().collect();

        for message in application_messages {

            println!("Got application message from {} : {:?}", message.connection, message.data);
        }

        thread::sleep(time::Duration::from_millis(2000));
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