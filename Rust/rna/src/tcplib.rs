use std::net::TcpStream;
use std::fmt;
use std::io::{Read, Write};

extern crate uuid;
use uuid::Uuid;

const TCP_BUFF: usize = 2048;

/// Errors that might arise in tcp reading / writing
#[derive(Debug)]
pub enum TcpError {
    ErrorReading(&'static str),
    ErrorWriting(&'static str),
    MaximumBytesExceeded
}

impl fmt::Display for TcpError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {

            TcpError::ErrorWriting(ref cause) => write!(f, "Error Writing: {}", cause),
            TcpError::ErrorReading(ref cause) => write!(f, "Error Reading: {}", cause),
            TcpError::MaximumBytesExceeded => {
                write!(f, "Client attempted to send more bytes than receiver permits")
            },
        }
    }
}

// Generate a connection id
fn generate_txid() -> String {
    Uuid::new_v4().to_hyphenated_ref().to_string()
}

/// Trait for receiving TCP data
pub trait TcpReceiver {
    fn maximum_bytes(&self) -> u64;
    fn begin_receive(&mut self, id: &String);
    fn bytes_received(&mut self, id: &String, data: &Vec<u8>);
    fn receive_complete(&mut self, id: &String);
}

/// Demonstration / testing receiver
pub struct EchoReceiver {}

impl TcpReceiver for EchoReceiver {
    fn maximum_bytes(&self) -> u64{
        10_000_000_000
    }

    fn begin_receive(&mut self, id: &String) {
        println!("New receive started with id: {}", id);
    }

    fn bytes_received(&mut self, id: &String, data: &Vec<u8>) {
        println!("Echo receiver got : {} bytes for stream {} ", data.len(), id);
    }

    fn receive_complete(&mut self, id: &String) {
        println!("All data for {} has been received", id);
    }
}

/// Inbound TCP structure that contains receiver for sending data to
pub struct TcpInbound <'a> {
    receiver : &'a mut dyn TcpReceiver, 
}

impl <'a> TcpInbound <'a> {

    /// Create a new TcpInbound object given a receiver
    pub fn new(receiver: &'a mut dyn TcpReceiver) -> Self {
        Self {
            receiver: receiver
        }
    }

    /// Read data from a TCP Stream with a rna writer sending the data
    pub fn read_stream (&mut self, stream: &mut TcpStream) -> Result<(), TcpError> {

        // Get number of chunks to be expected
        let mut chunks_expected = [0; 8];
        if let Err(_) = stream.read(&mut chunks_expected) {
            return Err(TcpError::ErrorReading("Error reading chunks expected"));
        }
        let num_chunks_expected = u64::from_le_bytes(chunks_expected);
        drop(chunks_expected);

        // Get trailing amount of data that will exist in last buffer
        let mut trail_expected = [0; 8];
        if let Err(_) = stream.read(&mut trail_expected) {
            return Err(TcpError::ErrorReading("Error reading trail expected"));
        }
        let num_trail_expected = u64::from_le_bytes(trail_expected);
        drop(trail_expected);

        // Ensure they won't be blasting us with more data than allowed
        if self.receiver.maximum_bytes() < (num_chunks_expected * TCP_BUFF as u64) - num_trail_expected {
            return Err(TcpError::MaximumBytesExceeded)
        }

        // Generate a transmission id
        let transmission_id = generate_txid();

        // Indicate that we are starting to recv data 
        self.receiver.begin_receive(&transmission_id);

        for citr in 0 .. num_chunks_expected {

            let mut current_chunk = vec![0u8; TCP_BUFF];

            if let Err(_) = stream.read_exact(&mut current_chunk) {
                return Err(TcpError::ErrorReading("Error reading chunk data"));
            }

            if citr == num_chunks_expected -1  {
                current_chunk.truncate( (current_chunk.len() as u64 - num_trail_expected) as usize);
            }

            // Send chunk of data up to receiver
            self.receiver.bytes_received(&transmission_id, &current_chunk);
        }

        // Indicate completion
        self.receiver.receive_complete(&transmission_id);

        Ok(())
    }
}

/// Send data to a rna reader
pub fn writer(stream: &mut TcpStream, data: Vec<u8>) -> Result<(), TcpError> {

    // Calculate chunk count
    let data_len = data.len() as u64;

    let chunk_count : u64 = (data_len as f32 / TCP_BUFF as f32).ceil() as u64;

    // Calculate trailing non-usable data
    let trail_count : u64 = if data_len < TCP_BUFF as u64 {
        TCP_BUFF as u64 - data_len
    } else {
        TCP_BUFF as u64 - data_len % TCP_BUFF as u64
    };

    // Write the chunk count
    if let Err(_) = stream.write(&chunk_count.to_le_bytes()) {
        return Err(TcpError::ErrorWriting("Unable to write expected chunk count"));
    }
    
    // Write the trail count
    if let Err(_) = stream.write(&trail_count.to_le_bytes()) {
        return Err(TcpError::ErrorWriting("Unable to write expected trail count"));
    }

    // Send the chunks
    for chunk in data.chunks(TCP_BUFF) {

        // If it is the last chunk we need to pad the buffer so the receiver can get all of the data
        if chunk.len() < TCP_BUFF{

            let mut buffed_chunk = Vec::new();
            buffed_chunk.extend(chunk.clone());

            for _ in 0 .. TCP_BUFF - chunk.len() {
                buffed_chunk.push(1);
            }

            if let Err(_) = stream.write(&buffed_chunk.as_ref()) {
                return Err(TcpError::ErrorWriting("Error writing chunk to server"));
            }

        } else {

            // Write a whole chunk over to the reader
            if let Err(_) = stream.write(chunk) {
                return Err(TcpError::ErrorWriting("Error writing chunk to server"));
            }
        }
    }

    Ok(())
}