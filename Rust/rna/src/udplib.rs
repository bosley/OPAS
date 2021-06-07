use std::net::UdpSocket;
use std::fmt;

use serde::{Serialize, Deserialize};

extern crate uuid;
use uuid::Uuid;

extern crate bincode;

/// Maximum chunk size sent over UDP 
pub const     UDP_CHUNK_SIZE: usize = 4096;

/// Size of receive buffer for UDP packets
pub const UDP_BUFFER:     usize = UDP_CHUNK_SIZE + 512;

/// Errors that might arise in udp reading / writing
#[derive(Debug)]
pub enum UdpError {
    ErrorReading(&'static str),
    ErrorWriting(&'static str)
}

impl fmt::Display for UdpError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {

            UdpError::ErrorWriting(ref cause) => write!(f, "Error Writing: {}", cause),
            UdpError::ErrorReading(ref cause) => write!(f, "Error Reading: {}", cause)
        }
    }
}

// Generate a connection id
fn generate_txid() -> String {
    Uuid::new_v4().to_hyphenated_ref().to_string()
}

// For identifying packets in a udp stream
#[derive(Serialize, Deserialize, PartialEq, Debug)]
struct UdpPacket {
    id:     String,
    index:  u64,
    last:   bool,
    data:   Vec<u8>
}

/// Trait for receiving UDP data
pub trait UdpReceiver {
    fn begin_receive(&mut self, id: &String);
    fn bytes_received(&mut self, id: &String, data: &Vec<u8>);
    fn receive_complete(&mut self, id: &String);
}

/// Demonstration / testing receiver
pub struct EchoReceiver {}

impl UdpReceiver for EchoReceiver {

    fn begin_receive(&mut self, id: &String) {
        println!("New receive started with id: {}", id);
    }

    fn bytes_received(&mut self, id: &String, data: &Vec<u8>) {
        println!("Echo receiver got : {} bytes for stream {}", data.len(), id);
    }

    fn receive_complete(&mut self, id: &String) {
        println!("All data for {} has been received", id);
    }
}

/// Inbound UDP structure that contains receiver for sending data to
pub struct UdpInbound <'a> {
    receiver : &'a mut dyn UdpReceiver, 
}

impl <'a> UdpInbound <'a> {

    /// Create a new UdpInbound object given a receiver
    pub fn new(receiver: &'a mut dyn UdpReceiver) -> Self {
        Self {
            receiver: receiver
        }
    }

    /// Read data from a UDP Stream with a rna writer sending the data
    pub fn read_socket_data (&mut self, num_bytes: usize, udp_raw_data: Vec<u8>) -> Result<(), UdpError> {

        let actual_bytes: Vec<u8> = udp_raw_data.iter().take(num_bytes).cloned().collect();

        let packet_data : UdpPacket = match bincode::deserialize(&actual_bytes) {
            Ok(dec) => dec,
            Err(_)  => {
                return Err(UdpError::ErrorWriting("Unable to deserialize inbound packet"));
            }
        };

        if packet_data.index == 0 {
            self.receiver.begin_receive(&packet_data.id);
        }

        self.receiver.bytes_received(&packet_data.id, &packet_data.data);

        if packet_data.last {
            self.receiver.receive_complete(&packet_data.id);
        }
        Ok(())
    }
}

/// Send data to a rna reader
pub fn writer(socket: &UdpSocket, destination: String, data: Vec<u8>) -> Result<(), UdpError> {

    let data_len = data.len() as u64;

    let chunk_count : u64 = (data_len as f32 / UDP_CHUNK_SIZE as f32).ceil() as u64;

    let mut index = 0;

    let transit_id = generate_txid();

    // Send the chunks
    for chunk in data.chunks(UDP_CHUNK_SIZE) {

        let packet = UdpPacket {
            id:    transit_id.clone(),
            index: index,
            last:  if index == chunk_count-1 {
                true
            } else {
                false
            },
            data: chunk.to_vec()
        };

        let encoded = match bincode::serialize(&packet) {
            Ok(enc) => enc, 
            Err(_)  => {
                return Err(UdpError::ErrorWriting("Could not serialize outbound packet"));
            }
        };

        if let Err(_) = socket.send_to(&encoded.to_vec(), destination.to_string()) {
            return Err(UdpError::ErrorWriting("Unable to send udp packet"));
        }

        index += 1;
    }

    Ok(())
}