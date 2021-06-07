extern crate coal_lang;
//extern crate coal_engine;

extern crate clap;
use clap::{Arg, App};

fn main() {
    let matches = App::new("Micron")
                          .version("0.1.0")
                          .author("Josh A. Bosley <bosley117@gmail.com>")
                          .about("Runs Coal programs")
                          .arg(Arg::with_name("file")
                               .help("Sets the input file to use")
                               .required(true)
                               .index(1))
                          .get_matches();


    // Using the importer we try to bring in the file and have the parser make something
    // the engine can jive with
    let source_tree = match coal_lang::from_file(&matches.value_of("file").unwrap().to_string()) {
        Ok(result) => { result },
        Err(e)     => {
            panic!("File Error: {:?}", e);
        }
    };

    /*
    let mut engine = coal_engine::Engine::new();

    if let Some(err) = engine.load(source_tree) {
        panic!("Error: {:?}", err);
    }

    if let Some(err) = engine.start() {
        panic!("Error: {:?}", err);
    }
*/
    println!("Got: {:?}", source_tree);
}