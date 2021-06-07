
extern crate rna;

use std::env;
use std::net::{TcpListener, TcpStream};
use rand::{ Rng, thread_rng };

fn main() {

    let args: Vec<String> = env::args().collect();

    if args.len() == 2 {
        if &args[1] == "client" {
            return run_client();
        }

        if &args[1] == "server" {
            return run_server();
        }
    }

    println!("Usage: {} [client | server]", &args[0]);
}

fn run_client() {
    let mut stream = match TcpStream::connect("127.0.0.1:34254"){
        Ok(s) => s,
        Err(e) => {
            eprintln!("Could not connect : {}", e); 
            return;
        }
    };

    let mut rng = thread_rng();

    let data: Vec<u8> = (0..8500).map(|_| {
        rng.gen_range(0, 250)
    }).collect();

    match rna::tcplib::writer(&mut stream, data) {
        Ok(_) => {
            println!("Success!");
        },

        Err(e) => {
            eprintln!("Error: {}", e)
        }
    }
}

fn run_server() {

    let listener = match TcpListener::bind("127.0.0.1:34254") {
        Ok(l) => l,
        Err(e) => {
            eprintln!("Error: {}", e);
            return;
        }
    };

    let mut echo_receiver = rna::tcplib::EchoReceiver{};

    let mut tcp_inbound = rna::tcplib::TcpInbound::new(&mut echo_receiver);

    // accept connections and process them serially
    for stream in listener.incoming() {

        let data_stream = match stream {
            Ok(s) => s,
            Err(e) => {
                eprintln!("Error matching stream: {}", e);
                continue
            }
        };

        let mut sb  = data_stream;

        match tcp_inbound.read_stream(&mut sb) {
            Ok(_) => println!("Success!"),
            Err(e) => {
                eprintln!("Error: {}", e);
            }
        }
    }
}