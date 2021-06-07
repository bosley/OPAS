
extern crate rna;

use std::env;
use std::net::UdpSocket;
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
    let mut rng = thread_rng();

    /*
        Due to the unreliability of UDP, if the data sent is larger
        than udplib::UDP_CHUNK_SIZE it is possible that data is lost, 
        and a that the given receiver object doesn't get the
        receive_complete callback.

        It is also possible that begin_receive doesn't get called, and
        it is also possible that data in-between first and last isn't 
        received. If using udp, it is recommened to stay within the
        constraint of udplib::UDP_CHUNK_SIZE to ensure that if data does
        in-fact go through all methods of the receiver will be triggered.
    */
    let data: Vec<u8> = (0..2048).map(|_| {
        rng.gen_range(0, 250)
    }).collect();

    let socket = UdpSocket::bind("127.0.0.1:3400").expect("couldn't bind to address");

    match rna::udplib::writer(&socket, "127.0.0.1:34254".to_string(), data.to_vec()) {
        Ok(_) => {
            println!("Success!");
        },

        Err(e) => {
            eprintln!("Error: {}", e)
        }
    }
}

fn run_server() {
   
    let mut echo_receiver = rna::udplib::EchoReceiver{};
    let mut udp_inbound = rna::udplib::UdpInbound::new(&mut echo_receiver);

    loop {
        let socket = match UdpSocket::bind("127.0.0.1:34254") {
            Ok(s) => s,
            Err(e) => {
                eprintln!("Error: {}", e);
                return;
            }
        };

        let mut buf = [0; rna::udplib::UDP_BUFFER];
        match socket.recv(&mut buf) {
            Ok(num_bytes) => {
                if let Err(e) = udp_inbound.read_socket_data(num_bytes, buf.to_vec()) {
                    eprintln!("Error: {}", e);
                }
            },
            Err(_) => {
                continue;
            }
        }
    }
}