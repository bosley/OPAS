


<p align="center">
  <img width="100" height="100" src="https://img.icons8.com/pastel-glyph/64/000000/dna-helix.png">
</p>
<b> <h1> <p align="center">  RNA  </p> </h1> </b>

# Rust Networking Accessories [![Build Status](https://travis-ci.com/joshbosley/rna.svg?branch=master)](https://travis-ci.com/joshbosley/rna)


### Simple utilities for making networked applications. 

This library makes sending large amounts of data over tcp/udp really easy.

On one side, you create a tcp/udp receiver that implements methods for the library to indicate when a transmission has started, when data comes in, and when a transmission ends. Each new transmission is given a unique id so when multiple transmissions are coming-in their stream fragments can be easily identified.  

### Simple Transmission!

```rust
    // TCP -------------------------------------\/ Can be any length
    match rna::tcplib::writer(&mut tcp_stream, data) {
        Ok(_) => {
            println!("Success!");
        },

        Err(e) => {
            eprintln!("Error: {}", e)
        }
    }

    // UDP
    let socket = UdpSocket::bind("127.0.0.1:3400").expect("couldn't bind to address");
    match rna::udplib::writer(&socket, "127.0.0.1:34254".to_string(), data.to_vec()) {
        Ok(_) => {
            println!("Success!");
        },

        Err(e) => {
            eprintln!("Error: {}", e)
        }
    }
```

### Examples

For detailed examples of usage, please see the examples directory. 

