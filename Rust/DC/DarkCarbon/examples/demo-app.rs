
extern crate darkcarbon;

extern crate nexus;

extern crate log;
extern crate simple_logger;
use log::Level;

use chrono;

extern crate ctrlc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::time;
use std::thread;

use std::{ 
    path::Path,
    env
 };

//  Network configuration for the demo applications
//
const NETWORK_NAME        : &str = "Nexus-Demo-Network";
const NETWORK_FILE        : &str = ".example_network.json";

// Nexus (node) one
//
const APP_ONE_NAME        : &str = "Nexus-App-One";
const APP_ONE_FILE        : &str = ".example_app_one.json";
const APP_ONE_CONN        : &str = "0.0.0.0:4096";

// Nexus (node) two 
//
const APP_TWO_NAME        : &str = "Nexus-App-Two";
const APP_TWO_FILE        : &str = ".example_app_two.json";
const APP_TWO_CONN        : &str = "0.0.0.0:4097";

// DC Application
//
const DC_APP_NAME         : &str = "DC-App";
const DC_APP_FILE         : &str = ".example_dc_app.json";
const DC_APP_DESC         : &str = "A demonstration application used for testing and showing off the Dark Carbon network functionality";


//
//  Entry
//
fn main() {
    simple_logger::init_with_level(Level::Debug).unwrap();

    let args: Vec<String> = env::args().collect();

    if args.len() == 2 {
        if &args[1] == "-h" || &args[1] == "--help" {
            show_usage();
            return;
        }

        if &args[1] == "-r" || &args[1] == "--run" {
            if ensure_files_exist() {
                run();
                return;

            } else {
                eprintln!("Files do not exist. Setup must be run first!");
                show_usage();
            }
            return;
        }
    }

    if args.len() == 4 {

        if &args[1] == "-s" || &args[1] == "--setup" {
            print!("Setting up...");


            let proto : nexus::network::TransferProtocol = if &args[2] == "tcp" {
                nexus::network::TransferProtocol::Tcp
            } else if &args[2] == "udp" {
                nexus::network::TransferProtocol::Udp
            } else {
                println!("Invalid protocol given (tcp | udp) available, you gave: {}", &args[2]);
                show_usage();
                return;
            };

            let app = if &args[3] == "app1" {
                true
            } else if &args[3] == "app2" {
                false
            } else {
                println!("Expected (app1 | app2) ... you gave {}", &args[3]);
                show_usage();
                return;
            };

            setup_apps(proto, app);
            println!("Complete!");
            return;
        }

    }

    show_usage();
}

//
//  Show usage
//
fn show_usage() {
    println!("\nNexus-App usage:");
    println!("\t[-h, --help ] to show help");
    println!("\t[-s, --setup (tcp | udp) (app1 | app2)] to run DC setup");
    println!("\t[-r, --run ] to run DC");
    println!("");
}

//
//  Run an app
//
fn run() {

    // Initialize the application and receive a control panel to interact with it
    //
    let mut app_control_panel : darkcarbon::app::ControlPanel = match darkcarbon::app::init_application(DC_APP_FILE.to_string()) {
        Ok(apc) => {
            apc
        }, 
        Err(e) => {
            eprintln!("Error running application : {}" , e);
            panic!("Unable to start the application!");
        }
    };

    // Start the application
    //
    if let Err(e) = app_control_panel.start() {
        panic!("Unable to start the application : {}", e);
    }

    //  Setup a handler to catch Ctrl+C so we can shutdown gracefully
    //
    println!("\n\tUse Ctrl-C to kill the dc app..\n");
    let running = Arc::new(AtomicBool::new(true));
    let r = running.clone();

    ctrlc::set_handler(move || {
        r.store(false, Ordering::SeqCst);
    }).expect("Error setting Ctrl-C handler");

    // Here we will loop and check for data from the network and respond however we feel like
    //
    while running.load(Ordering::SeqCst) {

        thread::sleep(time::Duration::from_secs(1));
    }

    app_control_panel.shutdown();

    println!("Shutdown complete");
}

//
//  Ensure the files exist
//
fn ensure_files_exist() -> bool {
    (
        Path::new(NETWORK_FILE).exists() &&
        Path::new(APP_ONE_FILE).exists() &&
        Path::new(APP_TWO_FILE).exists() &&
        Path::new(DC_APP_FILE).exists()      
    )
}

//
//  Create application files for app1 and app2
//
fn setup_apps(proto: nexus::network::TransferProtocol, use_app_one: bool) {

    // Create and write-out network file
    let network = nexus::system::generate_new_network(
        &NETWORK_NAME.to_string()
    );
    if let Err(_) = network.write_to_file(&NETWORK_FILE.to_string()){
        panic!("Unable to write network data to file.");
    }

    // Create node one
    let mut node_one = nexus::system::generate_new_local(
        &APP_ONE_NAME.to_string(),
        &APP_ONE_CONN.to_string()
    );
    node_one.file_name = APP_ONE_FILE.to_string();

    // Create node two
    let mut node_two = nexus::system::generate_new_local(
        &APP_TWO_NAME.to_string(),
        &APP_TWO_CONN.to_string()
    );
    node_two.file_name = APP_TWO_FILE.to_string();

    let mut connections : Vec<darkcarbon::system::NexusConnectionInfo> = Vec::new();

    if use_app_one {
        connections.push(darkcarbon::system::NexusConnectionInfo{
            nexus_local_file:   APP_ONE_FILE.to_string(),
            nexus_network_file: NETWORK_FILE.to_string(),
            nexus_proto:        proto
        });
    } else {
        connections.push(darkcarbon::system::NexusConnectionInfo{
            nexus_local_file:   APP_TWO_FILE.to_string(),
            nexus_network_file: NETWORK_FILE.to_string(),
            nexus_proto:        proto
        });
    }

    let mut tags : Vec<String> = Vec::new();
    tags.push("demo".to_string());
    tags.push("debug".to_string());
    tags.push("testing".to_string());

    let dc_application = darkcarbon::system::app_from_info(
        DC_APP_NAME.to_string(), 
        tags, 
        DC_APP_DESC.to_string(), 
        false,
        connections,
        DC_APP_FILE.to_string());

    // Add node two to node one
    node_one.add_peer(
        &nexus::system::Peer{
            id:         node_two.id.to_string(),
            name:       node_two.name.to_string(),
            connection: node_two.connection.to_string(),
            last_seen:  chrono::Utc::now()
        }
    );

    // Add node one to node two
    node_two.add_peer(
        &nexus::system::Peer{
            id:         node_one.id.to_string(),
            name:       node_one.name.to_string(),
            connection: node_one.connection.to_string(),
            last_seen:  chrono::Utc::now()
        }
    );

    // Write out nodes
    if let Err(_) = node_one.write_to_file(&APP_ONE_FILE.to_string()) {
        panic!("Unable to write out node one");
    }

    if let Err(_) = node_two.write_to_file(&APP_TWO_FILE.to_string()) {
        panic!("Unable to write out node two");
    }

    if let Err(_) = dc_application.write() {
        panic!("Unable to write DC Application");
    }

    //  We don't write-out the non-nonexistent (offline) node
    //  because it isn't loaded for any purpose
}