
/// Peer configuration methods
pub mod peer {  
    //
    /// Given a node name and connection information, make a peer
    //
    pub fn new(name: String, connection: String) -> Result<(String), &'static str> {

        let mut new_peer = name.to_string();
        new_peer.retain(|c| !c.is_whitespace());

        let mut peer = nexus::system::generate_new_local(
            &name.to_string(),
            &connection.to_string()
        );
        peer.file_name = format!("{}.json", new_peer.to_string()).to_string();

        if let Err(e) = peer.write_to_file(&peer.file_name.to_string()) {
            return Err(e);
        }

        Ok(peer.file_name)
    }

    //
    /// Add a remote peer to an existing local peer
    //
    pub fn add_remote(file: String, id: String, name: String, connection: String) -> Result<(String), &'static str> {

        let mut local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        local_node.add_peer(
            &nexus::system::Peer{
                id:         id.to_string(),
                name:       name.to_string(),
                connection: connection.to_string(),
                last_seen:  chrono::Utc::now()
            }
        );

        if let Err(_) = local_node.write_to_file(&file.to_string()) {
            return Err("Unable to write data to given file name");
        }

        Ok(
            format!("New peer '{}' was added to {}'s remote peers", id.to_string(), local_node.id.to_string()).to_string()
        )
    }

    //
    /// Remove a remote peer from an existing local peer
    //
    pub fn rem_remote(file: String, id: String) -> Result<(String), &'static str> {

        let mut local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        local_node.remove_peer(
            &nexus::system::Peer{
                id:         id.to_string(), // This is the only thing used to remove the peer
                name:       "Not-Required".to_string(),
                connection: "Not-Required".to_string(),
                last_seen:  chrono::Utc::now()
            }
        );

        if let Err(_) = local_node.write_to_file(&file.to_string()) {
            return Err("Unable to write data to given file name");
        }

        Ok(
            format!("If peer '{}' existed in {}'s remote peers, it doesn't anymore", id.to_string(), local_node.id.to_string()).to_string()
        )
    }

    //
    /// Change peer name
    //
    pub fn mod_name(file: String, name: String) -> Result<(String), &'static str> {

        let mut local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        local_node.name = name.to_string();

        if let Err(_) = local_node.write_to_file(&file.to_string()) {
            return Err("Unable to write data to given file name");
        }

        Ok(
            format!("Peer '{}' now has name {} and has maintained file name {}", 
                    local_node.id.to_string(), 
                    local_node.name.to_string(),
                    file.to_string()).to_string()
        )
    }

    //
    /// Change peer connection
    //
    pub fn mod_conn(file: String, conn: String) -> Result<(String), &'static str> {

        let mut local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        local_node.connection = conn.to_string();

        if let Err(_) = local_node.write_to_file(&file.to_string()) {
            return Err("Unable to write data to given file name");
        }

        Ok(
            format!("Peer '{}' now has connection {}", 
                    local_node.id.to_string(), 
                    local_node.connection.to_string()).to_string()
        )
    }

    //
    /// Change peer id
    //
    pub fn mod_id(file: String, id: String) -> Result<(String), &'static str> {

        let mut local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        let mut new_id = id.to_string();
        new_id.retain(|c| !c.is_whitespace());

        local_node.id = new_id.to_string();

        if let Err(_) = local_node.write_to_file(&file.to_string()) {
            return Err("Unable to write data to given file name");
        }

        Ok(
            format!("Peer id is now '{}'", 
                    local_node.id.to_string()).to_string()
        )
    }

    //
    /// List known peers
    //
    pub fn list(file: String) -> Result<(String), &'static str> {

        let local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        let peer_list = local_node.get_all_peers();

        println!("");
        println!("{}'s known peers", local_node.name);
        println!("-------------------------------");
        for peer in &peer_list {
            println!("Peer id:   {}",      peer.id);
            println!("Peer name: {}",      peer.name);
            println!("Peer conn: {}",      peer.connection);
            println!("Peer last seen: {}", peer.last_seen);
            println!("-------------------------------");
        }

        Ok(
            format!("Done showing {} known peers", &peer_list.len()).to_string()
        )
    }

    //
    /// Info about a peer
    //
    pub fn info(file: String) -> Result<(String), &'static str> {

        let local_node = match nexus::system::local_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        println!("");
        println!("Local peer's info");
        println!("-------------------------------");
        println!("id:  \t\t{}",      local_node.id);
        println!("name:\t\t{}",      local_node.name);
        println!("conn:\t\t{}",      local_node.connection);
        println!("listed file:\t{}", local_node.file_name);
        println!("knwon peers:\t{}", local_node.get_all_peers().len());
        println!("-------------------------------");

        Ok(
            "Done".to_string()
        )
    }
}

/// Network configuration methods
pub mod network {
    //
    /// Given a network name, leverage the nexus library to create a new network and write it to a file
    //
    pub fn new(network_name: String) -> Result<(String), &'static str> {

        let mut new_network = network_name.to_string();

        new_network.retain(|c| !c.is_whitespace());

        let network = nexus::system::generate_new_network(
            &network_name.to_string()
        );

        let new_file = format!("{}.json", new_network.to_ascii_lowercase());

        if let Err(_) = network.write_to_file(&new_file.to_string()){
            return Err("Unable to write network file.");
        }

        Ok(new_file)
    }

    //
    /// Rename a network
    //
    pub fn mod_name(file: String, name: String) -> Result<(String), &'static str> {

        let mut network = match nexus::system::network_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        let mut new_name = name.to_string();
        new_name.retain(|c| !c.is_whitespace());

        network.network_name = new_name.to_string();

        if let Err(_) = network.write_to_file(&file.to_string()){
            return Err("Unable to write network file.");
        }

        Ok(
            format!("Network name is now '{}'", 
                    network.network_name.to_string()).to_string()
        )
    }

    //
    /// Modify network id
    //
    pub fn mod_id(file: String, id: String) -> Result<(String), &'static str> {

        let mut network = match nexus::system::network_from_file(&file.to_string()) {
            Some(loc) => loc,
            None      => {
                return Err("Unable to load given file :(");
            }
        };

        let mut id = id.to_string();
        id.retain(|c| !c.is_whitespace());

        network.network_id = id.to_string();

        if let Err(_) = network.write_to_file(&file.to_string()){
            return Err("Unable to write network file.");
        }

        Ok(
            format!("Network id is now '{}'", 
                    network.network_id.to_string()).to_string()
        )
    }
}