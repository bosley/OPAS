extern crate derive_more;

pub mod ast;
mod token;
mod lexer;
mod semantics;

mod source;
use source::{Source, SourceError};

#[macro_use] 
extern crate lalrpop_util;

lalrpop_mod!(pub coal);

use std::fs::File;

#[derive(Debug)]
pub enum ParserError {

    UnableToOpenFile(String),
    UnableToLocateImport(String),
    LineReadError,
    ParseError,
    SemanticError
}

/// Parse a string of source
pub fn from_source(source: &String) -> Result<Vec<ast::Unit>, ParserError> {
    
    return SourceForge::new().from_source(source);
}

/// Parse an entire source file
pub fn from_file(file: &String) -> Result<Vec<ast::Unit>, ParserError> {

    return SourceForge::new().from_file(file);
}

/// Used to handle the bringing in of code and tracking code imported, along-with directories added for 
/// searching for imports. This needed to be an object so as things get recursively imported there was some
/// overall persistence of knowledge pertaining to things already brought in
struct SourceForge {

    known_directories: Vec<String>,     // Directories to search for imports
    imported_files: Vec<String>
}

impl SourceForge {

    /// New obj
    fn new() -> Self {
        Self {
            known_directories: Vec::new(),
            imported_files: Vec::new()
        }
    }

    /// Add a directory to search for imports (does not check for unique entries yet)
    fn add_dir(&mut self, dir: &String) {

        self.known_directories.push(dir.clone());
    }

    /// Check if a file was imported 
    fn was_file_imported(&self, file: &String) -> bool {
        for f in self.imported_files.iter() {
            if f == file {
                return true;
            }
        }
        false
    }

    /// Searches through known directories for the file given
    fn find_file(&mut self, file: &String) -> Option<String> {

        // Try the thing its self first
        match File::open(file) {
            Ok(_)  => { 
                self.imported_files.push(file.clone());
                return Some(file.clone()); 
            } 
            Err(_) => {}
        };

        for path in self.known_directories.iter() {

            let path_str = path.clone() + file.as_str();

            // println!("Looking in at : {}", &path_str);

            // Try the thing its self first
            match File::open(&path_str) {
                Ok(_)  => { 
                    self.imported_files.push(file.clone());
                    return Some(path_str); 
                } 
                Err(_) => {}
            };
        }
        None
    }

    // Parse a string of source
    fn from_source(&mut self, source: &String) -> Result<Vec<ast::Unit>, ParserError> {

        let mut source_man : Source = Source::from_string(source);

        return self.parse(&mut source_man);
    }

    /// Parse an entire source file
    fn from_file(&mut self, file: &String) -> Result<Vec<ast::Unit>, ParserError> {

        let mut source_man : Source = match Source::from_file(file) {
            Ok(s) => { s }
            Err(e) => {
                match e {
                    SourceError::LineReadError => return Err(ParserError::LineReadError),
                    SourceError::UnableToOpenFile => return Err(ParserError::UnableToOpenFile(file.clone()))
                }
            }
        };

        return self.parse(&mut source_man);
    }

    /// Parse the source that was loaded by source manager
    fn parse(&mut self, source_man: &mut Source) -> Result<Vec<ast::Unit>, ParserError> {

        // Run the source through lalrpop for stage 1
        let mut file_contents = match coal::ProgramParser::new().parse(
            lexer::Lexer::new(source_man.get_source())) {
            Ok(units)  => { units }
            Err(e) => { 

                source_man.show_parser_error(e);
                return Err(ParserError::ParseError);
            }
        };

        // Tag the units with the current file name in case there is an error
        for unit in file_contents.units.iter_mut() {

            unit.file = source_man.current_file.clone();
        }

        // Go through all 'dir' directives to add the directory to the places to search for code
        for dir in file_contents.dirs.iter() {

            self.add_dir(dir);
        }

        // Recurse adding imported files
        for import in file_contents.imports.iter() {

            // Make sure we don't bring things in too many times or get caught in a loop
            if self.was_file_imported(&import.file_name) {

                //println!("Item already in!");
                continue;
            }

            // First found first used for pathing
            if let Some(file_path) = self.find_file(&import.file_name) {
                
                // Once we have the file, call on self with indirect recursion to load the contents 
                let mut imported_contents = match self.from_file(&file_path) {

                    Ok(units) => { units },
                    Err(e)    => {
                        println!("Unable to import file ({})!  >> {:?}\n", import.file_name, e);
                        return Err(ParserError::UnableToOpenFile(import.file_name.clone()));
                    }
                };

                // Once all data pertaining to the import has loaded, add the contents to the original entry file's contents!
                file_contents.units.append(&mut imported_contents);
            } else {

                // Fail if something is wrong
                return Err(ParserError::UnableToLocateImport(import.file_name.clone()));
            };

        }

        // Now that all the file information has been loaded in, its time for some semantic analysis

        let mut semantic_analyzer = semantics::Semantics::new(&file_contents.units);

        match semantic_analyzer.analyze() {

            Some(semantic_error) => {
                source_man.show_semantic_error(semantic_error);
                return Err(ParserError::SemanticError);
            }

            None => {
                return Ok(file_contents.units);
            }
        }
    }
}

