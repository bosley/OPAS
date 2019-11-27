

extern crate rna;
use rna::tcplib::TcpReceiver;

extern crate serde;
use serde::{Deserialize, Serialize};

use crate::common;
use crate::datastore;

pub struct NodeReceiver {
    aggregated_data:          Vec<u8> 
}

fn new_node_receiver() -> NodeReceiver {
    NodeReceiver{
        aggregated_data: Vec::new()
    }
}

impl TcpReceiver for NodeReceiver {
    fn maximum_bytes(&self) -> u64{
        2_000_000   // Limit data transfer to 2MB per message
    }

    fn begin_receive(&mut self, _id: &String) {
        //debug!("New receive started with id: {}", id);
    }

    fn bytes_received(&mut self, _id: &String, data: &Vec<u8>) {
        //debug!("Node receiver got : {} bytes for stream {} ", data.len(), id);
        self.aggregated_data.extend(data.clone());
    }

    fn receive_complete(&mut self, _id: &String) {
        //debug!("All data for {} has been received", id);

        let deserialized_data : common::NodeToNodeTransfer =  match serde_json::from_slice(
            self.aggregated_data.as_ref()
        ){
            Ok(deserialized) => {
                deserialized
            },
            Err(e) => {
                warn!("Unable to deserialize vector data with length {}. Error {}", 
                        self.aggregated_data.len(),
                        e);
                return;
            }
        };

      //  if let Err(e) = self.gateway_channel_sender.send(deserialized_data.clone()) {
      //      warn!("Error placing aggregated data into gateway [{}]'s sender", e);
      //  }
    }
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Node {
    name:        String,
    description: String,
    gateway:     String,
    id:          String,
    connection:  String
}

/// Load a node from file
pub fn load_node(file: &String) -> Result<Node, &'static str> {

    let node_ds = match datastore::node_load(file.to_string()) {
        Ok(ds) => {
            ds
        },
        Err(e) => {
            return Err(e);
        }
    };

    Ok(Node{
        name:           node_ds.name,
        description:    node_ds.desc,
        gateway:        node_ds.gateway,
        id:             node_ds.id,
        connection:     node_ds.connection
    })
}

impl Node {

    pub fn start(&mut self, threads: usize) -> Result<(), &'static str> {

        warn!("Start called on node with {} threads, but start is not yet implemented", threads);

        Ok(())
    }
}

