/*

    A demonstration of the basic in-memory key-value data store

*/

use jewel_db;

use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
struct User {
    id:            u64,
    username:   String,
    email:      String,
    phone:      String
}

//  Helper to generate a small list of users
//
fn generate_user_list() -> Vec<User> {

    let mut users : Vec<User> = Vec::new();

    users.push(User{ 
        id:       0, 
        username: "powerman77".to_string(), 
        email:    "pman@gmail.com".to_string(),
        phone:    "906-111-1111".to_string()  
    });
    users.push(User{ 
        id:       1, 
        username: "goldendog".to_string(), 
        email:    "gdog@yahoo.com".to_string(),
        phone:    "853-111-1112".to_string()  
    });
    users.push(User{ 
        id:       2, 
        username: "sateshGG".to_string(), 
        email:    "sgg777@gmail.com".to_string(),
        phone:    "677-111-1444".to_string()  
    });
    users.push(User{ 
        id:       3, 
        username: "UBAHat".to_string(), 
        email:    "ubhat432@hotmail.com".to_string(),
        phone:    "989-800-1221".to_string()  
    });

    users
}

//  Main
//
fn main() {

    // Generate a user list
    let user_list = generate_user_list();

    // Create a settings
    let mut default_settings = jewel_db::basic::Settings::default();
    default_settings.data_store = ".each-user-list.db".to_string();

    // Load jewel core
    let mut jewel_core = match jewel_db::basic::load_database(&default_settings){
        Ok(core) => {
            core
        },
        Err(e) => {
            panic!("Unable to load jewel core : {}", e);
        }
    };

    // Serialize and store each user into jewel core
    for user in user_list.iter() {

        let serialized_user = match serde_json::to_vec(&user) {
            Ok(data) => {
                data
            },
            Err(e) => {
                panic!("Unable to serialize user ! {}", e);
            }
        };

        println!("Storing: {}", user.username);
        jewel_core.set(&user.username, &serialized_user);
    }

    if let Err(e) = jewel_core.full_save() {
        panic!("Unable to save : {}", e);
    }

    // Drop to reload
    drop(jewel_core);

    println!("Loading database...");

    // Reload database from file
    let jewel_core = match jewel_db::basic::load_database(&default_settings){
        Ok(core) => {
            core
        },
        Err(e) => {
            panic!("Unable to load jewel core : {}", e);
        }
    };

    // Grab each user from jewel and deserialize
    for user in user_list.iter() {

        match jewel_core.get(&user.username) {
            Ok(retrieved_val) => {

                let deserialized: User =  match serde_json::from_slice(&retrieved_val) {
                    Ok(deserialized) => {
                        deserialized
                    },
                    Err(e) => {
                        panic!("Error deserializing store user data : {}", e);
                    }
                };

                println!("Username: {} => {:?}", user.username, deserialized);
            },
            Err(e) => {
                panic!("Unable to user by username : {}", e);
            }
        };
    }
}
