extern crate serde;
extern crate serde_json;
extern crate bincode;

use std::io::{ErrorKind, Read, Write};
use std::net::{TcpListener, TcpStream, Shutdown};
use std::convert::TryInto;
use std::boxed::Box;
use std::sync::RwLock;
use std::thread;
use threadpool::ThreadPool;

use crate::framework;

lazy_static! {
    static ref RUN_SERVER: RwLock<bool> = RwLock::new(true);
}

// -------------------------------------------------------------
///  Stop serving 
// -------------------------------------------------------------

pub fn stop() {

    let mut s = RUN_SERVER.write().unwrap();
    *s = false.to_owned();
}

// -------------------------------------------------------------
///  Create a server thread given a connection string ex: 127.0.0.1:3030
// -------------------------------------------------------------

pub fn serve_threaded(conn: String) {

    thread::spawn(move || {
        match serve(conn) {
            Ok(_) => {
                info!("Server shutdown");
                return;
            }
            Err(e) => {
                info!("Server ended in error : {}", e);
                return;
            }
        }
    });
}

// -------------------------------------------------------------
///  Run the server given a connection string ex: 127.0.0.1:3030
// -------------------------------------------------------------

pub fn serve(conn: String) -> Result<bool, &'static str> {
    
    let listener = TcpListener::bind(conn).unwrap();

    listener.set_ttl(100).expect("could not set TTL");
    listener.set_nonblocking(true).expect("Cannot set non-blocking");

    let pool = ThreadPool::new(framework::THREAD_LIMIT);

    for stream in listener.incoming() {

        // Check for shutdown request
        //
        match RUN_SERVER.try_read() {
            Ok(n) => {
                if *n == false {
                    return Ok(true);
                }
            }
            Err(_) => continue,
        };

        //  Check the stream for something and process if ok
        //
        match stream {
            Ok(s) => {
                s.set_read_timeout(Some(framework::READ_TIMEOUT)).expect("set_read_timeout call failed");
                
                pool.execute(move|| {
                    handle_client(s);
                    debug!("Client disconnected");
                });
            }
            Err(ref e) if e.kind() == ErrorKind::WouldBlock => {
                continue;
            }
            Err(e) => {
                warn!("{}", e);
                return Err("Encountered IO Error");
            }
        }

        pool.join();
    }

    Ok(true)
}

// -------------------------------------------------------------
//  handle a TCP client
// -------------------------------------------------------------

fn handle_client(mut stream: TcpStream) {

    // --- 0 : Get the number of data blocks they want to send
    let mut buff = [0; framework::NET_DATA_LEN];
    if let Err(_) = stream.read(&mut buff) {
        debug!("Unable to read initial block buffer from {:?}", stream.peer_addr());
        return
    }
    let bytes_expected = u64::from_le_bytes(buff);

    drop(buff);

    // --- 1 : Inform them we got it
    if let Err(_) = stream.write(&framework::NET_RESP_VALID) {
        debug!("Unable to write validation buffer to client!");
        return
    }

    // --- 2: Start getting the blocks
    let mut byte_buff : Box<Vec<u8>> = Box::new(Vec::with_capacity(bytes_expected.try_into().unwrap()));
    
    for x in 0..(bytes_expected as u64) {
        
        let mut ephemeral_buff = vec![0u8; 1];

        if let Err(_) = stream.read_exact(&mut ephemeral_buff) {
            debug!("Unable to read byte #{} from data from client!", x);

            if let Err(_) = stream.write(&framework::NET_RESP_INVALID) {
                return
            }
            return;
        }

        byte_buff.push(ephemeral_buff[0]);
    }
    drop(bytes_expected);

    // --- 3

    // Inform the client that we got the data
    //
    if let Err(_) = stream.write(&framework::NET_RESP_VALID) {
        debug!("Unable to write validation buffer to client!");
        return
    }

    // Deserialize the data
    //
    let str_data : String = 
        match bincode::deserialize(&byte_buff[..]) {
            Ok(s) => s,
            Err(_) => {
                if let Err(_) = stream.write(&framework::NET_RESP_INVALID) {
                    return
                }
                debug!("Error deserializing request from byte buffer");
                return;
            }
        };

    let frame : framework::Frame = match serde_json::from_str(&str_data) {
        Ok(n) => n,
        Err(_) => {
                if let Err(_) = stream.write(&framework::NET_RESP_INVALID) {
                    return
                }
                debug!("Error deserializing request from str");
                return;
        }
    };

    drop(byte_buff);

    // Handle the request 
    //
    let encoded = framework::execute(frame);

    // --- 4 : Send the number of data blocks

    if let Err(_) = stream.write(&encoded.len().to_le_bytes()) {
        warn!("Unable to write length of response to client");
        return
    }

    // --- 5 : Read response to ensure they got it
    let mut byte_buff = vec![0u8; 4];

    if let Err(_) = stream.read_exact(&mut byte_buff) {
        warn!("Unable to read size validation from client!");
        return;
    }

    if byte_buff == framework::NET_RESP_INVALID {
        warn!("Recvd the INVALID indication from client after sending length of method call resp");
        return;
    }

    if byte_buff != framework::NET_RESP_VALID {
        warn!("Didn't receive the VALID identifier from client after sending length of method call resp");
        return;
    }

    drop(byte_buff);

    // - 6 : Send the data
    for chunk in encoded.chunks(1) {
        if let Err(_) = stream.write(chunk) {
            return;
        }
    }

    drop(encoded);

    // --- 7 : Read response to ensure they got it
    let mut byte_buff = vec![0u8; 4];

    if let Err(_) = stream.read_exact(&mut byte_buff) {
        warn!("Unable to read size validation from client!");
        return;
    }

    if byte_buff == framework::NET_RESP_INVALID {
        warn!("Recvd the INVALID indication from client after sending length of method call resp");
        return;
    }

    if byte_buff != framework::NET_RESP_VALID {
        warn!("Didn't receive the VALID identifier from client after sending length of method call resp");
        return;
    }

    drop(byte_buff);
    
    stream.flush().unwrap();
    stream.shutdown(Shutdown::Both).unwrap();
}
