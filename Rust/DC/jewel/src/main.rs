/*
    💎 The Jewel CLI 💎

*/
extern crate tui;
extern crate termion;
extern crate clap;
use clap::{Arg, App, SubCommand};

use libjewel;

fn main() {

    let matches = App::new(libjewel::APPLICATION_NAME)
                    .version(libjewel::VERSION)
                    .author(libjewel::AUTHOR)
                    .about(libjewel::ABOUT)

                    .subcommand(SubCommand::with_name("net-new-network")
                                .about("Create a new network")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("name")
                                    .short("n")
                                    .long("name")
                                    .value_name("NAME")
                                    .help("Set the name of the network")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("net-mod-name")
                                .about("Modify a network's name")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the network config to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("name")
                                    .short("n")
                                    .long("name")
                                    .value_name("NAME")
                                    .help("The network's new name")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("net-mod-id")
                                .about("Modify a network's id")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the network config to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("id")
                                    .short("i")
                                    .long("id")
                                    .value_name("ID")
                                    .help("The network's new id")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("peer-new-peer")
                                .about("Create a new peer")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("name")
                                    .short("n")
                                    .long("name")
                                    .value_name("NAME")
                                    .help("Set the name of the peer")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("connection")
                                    .short("c")
                                    .long("conn")
                                    .value_name("CONNECTION")
                                    .help("Set the connection information for the peer")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("peer-add-remote")
                                .about("Add a 'known' remote to a stored peer")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("id")
                                    .short("i")
                                    .long("id")
                                    .value_name("PEERID")
                                    .help("The remote peer's id to add")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("name")
                                    .short("n")
                                    .long("name")
                                    .value_name("NAME")
                                    .help("The remote peer's name")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("connection")
                                    .short("c")
                                    .long("conn")
                                    .value_name("CONNECTION")
                                    .help("The remote peer's connection")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("peer-rem-remote")
                                .about("Remove a 'known' remote from a stored peer")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("id")
                                    .short("i")
                                    .long("id")
                                    .value_name("PEERID")
                                    .help("The remote peer's id to remove")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("peer-mod-name")
                                .about("Modify a local peer's name")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("name")
                                    .short("n")
                                    .long("name")
                                    .value_name("NAME")
                                    .help("The local peer's new name")
                                    .required(true)
                                    .takes_value(true)))
                                    
                    .subcommand(SubCommand::with_name("peer-mod-conn")
                                .about("Modify a local peer's connection")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("conn")
                                    .short("c")
                                    .long("connection")
                                    .value_name("CONN")
                                    .help("The local peer's new name")
                                    .required(true)
                                    .takes_value(true)))
                                    
                    .subcommand(SubCommand::with_name("peer-mod-id")
                                .about("Modify a local peer's id")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to edit")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("id")
                                    .short("i")
                                    .long("id")
                                    .value_name("ID")
                                    .help("The local peer's new id")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("peer-list-known")
                                .about("List all remote peers known to a peer")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer to list")
                                    .required(true)
                                    .takes_value(true)))
                                    
                    .subcommand(SubCommand::with_name("peer-info")
                                .about("List info about peer from file")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Set the file of the peer")
                                    .required(true)
                                    .takes_value(true)))

                    .subcommand(SubCommand::with_name("nexus-viewer")
                                .about("View a nexus network data activity")
                                .version(libjewel::VERSION)
                                .arg(Arg::with_name("file")
                                    .short("f")
                                    .long("file")
                                    .value_name("FILE")
                                    .help("Network configuration file")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("conn")
                                    .short("c")
                                    .long("connection")
                                    .value_name("CONN")
                                    .help("The local peer's new name")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("proto")
                                    .short("p")
                                    .long("proto")
                                    .value_name("PROTO")
                                    .help("Protocol to use (tcp | udp)")
                                    .required(true)
                                    .takes_value(true))
                                .arg(Arg::with_name("remote")
                                    .short("r")
                                    .long("remote")
                                    .value_name("REMOTE")
                                    .help("Known remote node")
                                    .required(true)
                                    .takes_value(true)))
                    .get_matches();

    /*
            New Network
    */
    if let Some(new_matches) = matches.subcommand_matches("net-new-network") {

        match libjewel::network::new(
            new_matches.value_of("name").unwrap().to_string()
        ) {
            Ok(file) => {
                println!("New network written to: {}", file.to_string());
            },

            Err(e) => {
                println!("Error creating new network: {}", e);
            }
        }
        return;
    }

    /*
        Modify network name
    */
    if let Some(new_matches) = matches.subcommand_matches("net-mod-name") {

        match libjewel::network::mod_name(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("name").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error editing network: {}", e);
            }
        }
        return;
    }

    /*
        Modify network id
    */
    if let Some(new_matches) = matches.subcommand_matches("net-mod-id") {

        match libjewel::network::mod_id(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("id").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error editing network: {}", e);
            }
        }
        return;
    }

    /*
            New peer
    */
    if let Some(new_matches) = matches.subcommand_matches("new-peer") {

        match libjewel::peer::new(
            new_matches.value_of("name").unwrap().to_string(),
            new_matches.value_of("connection").unwrap().to_string()
        ) {
            Ok(file) => {
                println!("New peer written to: {}", file.to_string());
            },

            Err(e) => {
                println!("Error creating new peer: {}", e);
            }
        }
        return;
    }

    /*
        Add remote
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-add-remote") {

        match libjewel::peer::add_remote(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("id").unwrap().to_string(),
            new_matches.value_of("name").unwrap().to_string(),
            new_matches.value_of("connection").unwrap().to_string()
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error adding peer: {}", e);
            }
        }
        return;
    }

    /*
        Remove remote
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-rem-remote") {

        match libjewel::peer::rem_remote(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("id").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error removing peer: {}", e);
            }
        }
        return;
    }

    /*
        Modify peer name
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-mod-name") {

        match libjewel::peer::mod_name(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("name").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error editing peer: {}", e);
            }
        }
        return;
    }
    
    /*
        Modify peer connection
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-mod-conn") {

        match libjewel::peer::mod_conn(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("conn").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error editing peer: {}", e);
            }
        }
        return;
    }
    
    /*
        Modify peer id
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-mod-id") {

        match libjewel::peer::mod_id(
            new_matches.value_of("file").unwrap().to_string(),
            new_matches.value_of("id").unwrap().to_string(),
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error editing peer: {}", e);
            }
        }
        return;
    }
    
    /*
        List peer's known peers
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-list-known") {

        match libjewel::peer::list(
            new_matches.value_of("file").unwrap().to_string()
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error listing known remote peers: {}", e);
            }
        }
        return;
    }
    
    /*
        List peer's information
    */
    if let Some(new_matches) = matches.subcommand_matches("peer-info") {

        match libjewel::peer::info(
            new_matches.value_of("file").unwrap().to_string()
        ) {
            Ok(message) => {
                println!("{}", message);
            },

            Err(e) => {
                println!("Error listing known remote peers: {}", e);
            }
        }
        return;
    }

    /*
        Nexus viewer
    */
    if let Some(new_matches) = matches.subcommand_matches("nexus-viewer") {

        let proto_match = new_matches.value_of("proto").unwrap().to_string();

        let proto : nexus::network::TransferProtocol = if proto_match == "tcp" {
            nexus::network::TransferProtocol::Tcp
        } else if proto_match == "udp" {
            nexus::network::TransferProtocol::Udp
        } else {
            println!("Invalid protocol given (tcp | udp) available, you gave: {}", proto_match);
            return;
        };


        match libjewel::run::nexus_network_viewer(
            &new_matches.value_of("conn").unwrap().to_string(),
            proto,
            &new_matches.value_of("file").unwrap().to_string(),
            &new_matches.value_of("remote").unwrap().to_string()
        ) {
            Ok(_) => {
                println!("Complete");
            },

            Err(e) => {
                println!("Error while attempting to view a nexus network: {}", e);
            }
        }
        return;
    }
    println!("💎 The Jewel CLI 💎");
    println!("");
    println!("Use -h or --help to see what you can do!");
}