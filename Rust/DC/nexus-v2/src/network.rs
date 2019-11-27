extern crate rna;

use rna::tcplib::{TcpReceiver};
use std::net::{ TcpListener, TcpStream };

use std::time;
use std::thread;
use std::sync::{Arc};
use std::sync::atomic::{AtomicBool, Ordering};
use std::io::ErrorKind;
use crossbeam::crossbeam_channel::{Sender};

use threadpool::ThreadPool;

extern crate serde;
use serde::{Deserialize, Serialize};

/// Transport message - Used only by the tcp sender and receiver to communicate information between eachother
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Transport {
    pub connection_info: String,                // Open and connectable ip:port of the sender so we can contact them later
    pub data:            Vec<u8>                // Actual data from the message; can be anything
}

/// Settings for starting-up TCP
pub struct TcpSettings {
    pub ttl:                u32,
    pub read_timeout:       time::Duration,
    pub connection:         String, 
    pub thread_pool_size:   usize,
    pub maximum_tcp_size:   u64,
    pub alive_flag:         Arc<AtomicBool>,
    pub completed_data_sender: Sender<Transport>
}

/// The RNA tcp receiver
struct NetworkReceiver {
    max_tcp_size:          u64,                 // Maximum number of bytes to receive in any given transmission
    data:                  Vec<u8>,             // Byte data of the message, should be a serialized Transport message
    completed_data_sender: Sender<Transport>
}

impl TcpReceiver for NetworkReceiver {
    fn maximum_bytes(&self) -> u64{
        self.max_tcp_size
    }

    fn begin_receive(&mut self, _id: &String) {
    }

    fn bytes_received(&mut self, _id: &String, data: &Vec<u8>) {
        self.data.extend(data.clone());
    }

    fn receive_complete(&mut self, _id: &String) {

        let deserialized_transport : Transport =  match serde_json::from_slice(
            self.data.as_ref()
        ){
            Ok(deserialized) => {
                deserialized
            },
            Err(e) => {
                warn!("Unable to deserialize vector data with length {}. Error {}", 
                        self.data.len(),
                        e);
                return;
            }
        };

        if let Err(e) = self.completed_data_sender.send(deserialized_transport) {
            warn!("Error placing aggregated tcp message into sender : {}", e);
        }
    }
}

/// Start a TCP receiver thread
pub fn get_receiver_thread(settings: TcpSettings) -> Option<thread::JoinHandle<()>> {

    Some(thread::spawn(move || {
        let listener = match TcpListener::bind(String::clone(&settings.connection)) {
            Ok(l)  => l,
            Err(e) => { panic!("Unable to bind TcpListener: {}", e) }
        };

        listener.set_ttl(settings.ttl).expect("could not set TTL");
        listener.set_nonblocking(true).expect("Cannot set non-blocking");

        let tcp_thread_pool = ThreadPool::new(settings.thread_pool_size.clone());

        'listener: for stream in listener.incoming() {

            if !settings.alive_flag.load(Ordering::SeqCst) {
                break 'listener;
            }

            let mut data_stream = match stream {
                Ok(s) => { s }
                Err(ref e) if e.kind() == ErrorKind::WouldBlock => {
                    continue;
                }
                Err(_) => {
                    error!("Io Error");
                    break 'listener;
                }
            };

            if let Err(e) = data_stream.set_read_timeout(
                Some(settings.read_timeout)) {
                    warn!("Unable to set read timeout on new stream: {}", e);
                }

            let data_sender         = settings.completed_data_sender.clone();
            let max_tcp_size        = settings.maximum_tcp_size.clone();

            // Thread off the connection to get data
            tcp_thread_pool.execute(move|| {

                let mut receiver_obj = NetworkReceiver{
                    max_tcp_size:           max_tcp_size,
                    data:                   Vec::new(),
                    completed_data_sender:  data_sender.clone()
                };

                let mut tcp_inbound = rna::tcplib::TcpInbound::new(&mut receiver_obj);

                match tcp_inbound.read_stream(&mut data_stream) {
                    Ok(_) => {
                        //debug!("tcp_inbound.read_stream() success");
                    },
                    Err(e) => {
                        warn!("{}", e);
                    }
                }
            }); // End tcp_thread_pool
        } // End stream in listener  
    }))
}

pub fn send(local: String, remote: String, data: Vec<u8>) -> Result<(), &'static str> {

    // Wrap data in transport

    let serialized_transport = match serde_json::to_vec(&Transport{
        connection_info: local,
        data:            data
    }) {
        Ok(st) => {
            st
        },
        Err(e) => {
            warn!("Unable to construct transport object in network send : {}", e);
            return Err("Error constructing transport message");
        }
    };

    let mut stream = match TcpStream::connect(remote.clone()){
        Ok(s) => s,
        Err(e) => {
            warn!("Could not connect to {} : {}", remote, e); 
            return Err("Could not connect to remote");
        }
    };

    match rna::tcplib::writer(&mut stream, serialized_transport) {
        Ok(_) => {
            //debug!("Success sending data to {}", remote);
        },

        Err(_) => {
            return Err("Error sending data");
        }
    }

    Ok(())
}