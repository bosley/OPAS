/*
    The gateway will facilitate connections to/from remote node clusters, and should be launched from this service.
    This service can also be used to generate node configurations to be used with the gateway.

    Notes:

        new --name <name of a new gateway>

        inject --gateway <name of gateway to inject into> --ip 192.168.1.28 --port 4096    // Add a remote gateway
        
        node --name <node name> --desc <node description> --gateway <name of gateway to generate node for>

        run --name <name> --ip <ip> --port <port> --level [info | debug | warn]
*/

extern crate nexus;

#[macro_use]
extern crate log;
extern crate simple_logger;
use log::Level;

extern crate ctrlc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;

extern crate clap;
use clap::{Arg, App, SubCommand};

// ---------------------------------------------------------------------------
/// Entry point into nexus service
// ---------------------------------------------------------------------------

fn main() {
    let matches = App::new("Nexus Service Application")
                    .version("1.0")
                    .author("Josh A. Bosley <bosley117@gmail.com>")
                    .about("Launch a gateway, spawn a node configuration file, etc")

                    .subcommand(SubCommand::with_name("new")
                        .about("Setup a new gateway to be launched")
                        .arg(Arg::with_name("name")
                            .long("name")
                            .help("Sets the name of the gateway")
                            .required(true)
                            .takes_value(true)))

                    .subcommand(SubCommand::with_name("inject")
                        .about("Inject information of a remote gateway into the local gateway")
                        .arg(Arg::with_name("gateway")
                            .long("gateway")
                            .help("Name of the local gateway of which to inject the connection")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("id")
                            .long("id")
                            .help("Sets the id of the remote gateway")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("ip")
                            .long("ip")
                            .help("Sets the ip of the remote gateway")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("port")
                            .long("port")
                            .help("Sets the port of the remote gateway")
                            .required(true)
                            .takes_value(true)))

                    .subcommand(SubCommand::with_name("node")
                        .about("Generate a new node to be connected to a local gateway")
                        .arg(Arg::with_name("ip")
                            .long("ip")
                            .help("Sets the ip of the node")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("port")
                            .long("port")
                            .help("Sets the port of the node")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("name")
                            .long("name")
                            .help("Name of the node to generate")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("desc")
                            .long("desc")
                            .help("Sets description of the node")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("gateway")
                            .long("gateway")
                            .help("Sets gateway to generate the node for")
                            .required(true)
                            .takes_value(true)))

                    .subcommand(SubCommand::with_name("run")
                        .about("Run gateway")
                        .arg(Arg::with_name("level")
                            .long("level")
                            .help("Sets the log level of the gateway")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("name")
                            .long("name")
                            .help("Sets the name of the gateway")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("ip")
                            .long("ip")
                            .help("Sets the ip address to use")
                            .required(true)
                            .takes_value(true))
                        .arg(Arg::with_name("port")
                            .long("port")
                            .help("Sets the port to use")
                            .required(true)
                            .takes_value(true)))
                    .get_matches();

    // Create new gateway
    if let Some(matches) = matches.subcommand_matches("new") {
        new_gateway(matches.value_of("name").unwrap().to_string());
        return;
    }

    // Inject a gateway connection
    if let Some(matches) = matches.subcommand_matches("inject") {
        inject_gateway(
            matches.value_of("gateway").unwrap().to_string(),
            matches.value_of("id").unwrap().to_string(),
            matches.value_of("ip").unwrap().to_string(),
            matches.value_of("port").unwrap().to_string());
        return;
    }

    // Creates a node config for a local gateway
    if let Some(matches) = matches.subcommand_matches("node") {
        new_node(
            matches.value_of("ip").unwrap().to_string(),
            matches.value_of("port").unwrap().to_string(),
            matches.value_of("name").unwrap().to_string(),
            matches.value_of("desc").unwrap().to_string(),
            matches.value_of("gateway").unwrap().to_string());
        return;
    }

    // Run a gateway
    if let Some(matches) = matches.subcommand_matches("run") {
        match run_gateway(
                    matches.value_of("name").unwrap().to_string(),
                    matches.value_of("ip").unwrap().to_string(),
                    matches.value_of("port").unwrap().to_string(),
                    matches.value_of("level").unwrap().to_string()){
                        Ok(_) => {
                            println!("Run complete");
                        },
                        Err(e) => {
                            eprintln!("{}", e);
                        }
                    }
        return;
    }

    eprintln!("No known arguments given. Use -h or --help to see what you can do.");
}

// ---------------------------------------------------------------------------
/// Create a new gateway and save it
// ---------------------------------------------------------------------------

fn new_gateway(name: String) {

    println!("Creating new gateway with name : {}", name);

    let gateway : nexus::Gateway = nexus::new_gateway(name);

    match gateway.save() {
        Ok(_) => {
            println!("Gateway Saved");
        },
        Err(e) => {
            println!("Error saving: {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
/// Inject a gateway into local gateway data storage
// ---------------------------------------------------------------------------

fn inject_gateway(name: String, id: String, ip: String, port: String) {

    let mut gateway : nexus::Gateway = match nexus::load_gateway(name.clone(), ip.clone(), port.clone(), 1){
        Ok(gate) => {
            gate
        },
        Err(e) => {
            println!("Error loading gateway {}. Error : {}", name, e);
            return;
        }
    };

    match gateway.inject_remote_gateway(id.clone(), format!("{}:{}", ip, port)) {
        Ok(_) => {
            println!("Gateway with id {} has been injected into local gateway {}", id, name);
        },
        Err(e) => {
            println!("Error injecting gateway : {}", e);
        }
    }

    match gateway.save() {
        Ok(_) => {
            println!("Gateway Saved");
        },
        Err(e) => {
            println!("Error saving: {}", e);
        }
    }
}

// ---------------------------------------------------------------------------
/// Create a new node and inject it into the local gateway
// ---------------------------------------------------------------------------

fn new_node(ip: String, port: String, name: String, desc: String, local_gateway: String) {

    let gate = match nexus::datastore::gateway_load_and_convert(format!("{}.json", local_gateway.to_string())) {
        Ok(g) => {
            g
        },
        Err(e) => {
            println!("Error loading gateway {} : {}", local_gateway, e);
            return;
        }
    };

    let ds = nexus::datastore::NodeDataStore{
        id:     nexus::common::generate_id(),
        name:   name.clone(),
        desc:   desc,
        connection: format!("{}:{}", ip, port),
        gateway: gate.id
    };

    match nexus::datastore::node_save(format!("node-{}.json", name), &ds) {
        Ok(_) => {
            println!("Node written to file")
        },
        Err(e) => {
            println!("Error writing node {} : {}", name, e);
            return;
        }
    }

    match nexus::datastore::gateway_insert_node(format!("{}.json", local_gateway), ds) {
        Ok(_) => {
            println!("Node injected into gateway");
        },
        Err(e) => {
            println!("Error injecting node into gateway : {}", e);
            return;
        }
    }
}

// ---------------------------------------------------------------------------
/// Run a gateway
// ---------------------------------------------------------------------------

fn run_gateway(name: String, ip: String, port: String, level: String) -> Result<(), &'static str> {

    if level == "info" {
        simple_logger::init_with_level(Level::Info).unwrap();
    } else if level == "debug" {
        simple_logger::init_with_level(Level::Debug).unwrap();
    } else if level == "warn" {
        simple_logger::init_with_level(Level::Warn).unwrap();
    } else {
        return Err("Incorrect log level given. Options are [info | debug | warn]")
    }
    
    info!("Nexus Service");

    let server_thread_count: usize = 10;

    let mut gateway : nexus::Gateway = nexus::load_gateway(name, ip, port, server_thread_count)?;

    match gateway.serve() {
        Ok(_) => {
            info!("Gateway started");
        },
        Err(e) => {
            warn!("Unable to start gateway : {}", e);
            return Err("Unable to start");
        }
    }

    let running = Arc::new(AtomicBool::new(true));
    let r = running.clone();

    ctrlc::set_handler(move || {
        r.store(false, Ordering::SeqCst);
    }).expect("Error setting Ctrl-C handler");

    println!("Use Ctrl-C to exit");

    while running.load(Ordering::SeqCst) {}

    println!("");

    match gateway.stop() {
        Ok(_) => {
            info!("Gateway has stopped");
        },
        Err(e) => {
            warn!("Unable to stop gateway : {}", e);
            return Err("Unable to stop");
        }
    }
    Ok(())
}