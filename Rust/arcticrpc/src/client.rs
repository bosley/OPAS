use std::net::{TcpStream, Shutdown};
use std::io::{Read, Write};
use std::convert::TryInto;
use std::boxed::Box;
extern crate serde_json;
use crate::framework;

pub fn call(conn: &String, method: String, params: serde_json::Value) -> Result< serde_json::Value, &'static str> {

    // Build call frame
    //
    let call_frame: framework::Frame = framework::Frame {
        called_method: method,
        called_params: params
    };

    // Make it a json 
    //
    let call_frame_json =  match serde_json::to_string(&call_frame) {
        Ok(n) => n,
        Err(_) => return Err("Error serializing struct to json before server call")
    };

    // Serialize it
    //
    let encoded: Vec<u8> = match bincode::serialize(&call_frame_json) {
        Ok(n) => n,
        Err(_) => return Err("Unable to encode call_frame for request")
    };

    // -------------------------------------------------------------------------

    // Connect
    //
    let mut stream = match TcpStream::connect(conn) {
        Ok(n) => n,
        Err(_) => return Err("Could not connect!")
    };

    // --- 0 : Send the number of data blocks
    
    if let Err(_) = stream.write(&encoded.len().to_le_bytes()) {
        return Err("Unable to write num blocks to server")
    }

    // --- 1 : Ensure they got it
    let mut byte_buff = vec![0u8; 4];

    if let Err(_) = stream.read_exact(&mut byte_buff) {
        return Err("Unable to read validation from server")
    }

    if byte_buff == framework::NET_RESP_INVALID {
        return Err("Received INVALID indication after sending data length to server")
    } 

    if byte_buff != framework::NET_RESP_VALID {
        return Err("Did not receive the VALID identifier from server after sending length")
    }
    drop(byte_buff);

    // --- 2: Start sending blocks

    for chunk in encoded.chunks(1) {
        if let Err(_) = stream.write(chunk) {
            return Err("Unable to write chunk")
        }
    }

    // --- 3 : Ensure they got it

    let mut byte_buff = vec![0u8; 4];

    if let Err(_) = stream.read_exact(&mut byte_buff) {
        return Err("Unable to read validation from server")
    }

    if byte_buff == framework::NET_RESP_INVALID {
        return Err("Received INVALID indication after sending data to server")
    } 

    if byte_buff != framework::NET_RESP_VALID {
        return Err("Did not receive the VALID identifier from server after sending data")
    }
    drop(byte_buff);

    // --- 4 : Get ready to receive data by getting a length
    let mut recv_len_buff = [0; framework::NET_DATA_LEN];

    if let Err(_) = stream.read(&mut recv_len_buff) {
        return Err("Unable to get size of response from server")
    }

    // --- 5 : Inform the server that we got the data len, and are expecting it
    if let Err(_) = stream.write(&framework::NET_RESP_VALID) {
        return Err("Unable to write size validation to server")
    }

    let bytes_expected = u64::from_le_bytes(recv_len_buff);

    // -- 6 : Receive the data
    let mut byte_buff : Box<Vec<u8>> = Box::new(Vec::with_capacity(bytes_expected.try_into().unwrap()));
    
    for x in 0..(bytes_expected) {
        
        let mut ephemeral_buff = vec![0u8; 1];

        if let Err(_) = stream.read_exact(&mut ephemeral_buff) {
            debug!("Unable to read byte #{} from data from server!", x);

            if let Err(_) = stream.write(&framework::NET_RESP_INVALID) {
                return Err("Unable to write INVALID upon data recv err")
            }
            return Err("Failed to write all bytes to server");
        }

        byte_buff.push(ephemeral_buff[0]);
    }

    // --- 7 : Inform the server that we got the data
    if let Err(_) = stream.write(&framework::NET_RESP_VALID) {
        return Err("Unable to write size validation to server")
    }
    
    // Close the stream - We don't need them anymore
    //
    stream.shutdown(Shutdown::Both).unwrap();

    let check_call : Box<Vec<u8>> = Box::new(framework::NET_NO_METHOD.to_vec());

    if byte_buff == check_call {
        return Err("Method not found");
    }

    // Deserialize the data
    //
    let str_data : String = 
        match bincode::deserialize(&byte_buff[..]) {
            Ok(s) => s,
            Err(_) => return Err("Error deserializing json response from server")
        };

    let frame_result : framework::FrameResult = match serde_json::from_str(&str_data) {
        Ok(n) => n,
        Err(_) => return Err("Error deserializing struct response from json")
    };

    Ok(frame_result.result_value)
}