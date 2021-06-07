use std::fs::File;
use std::io::{BufRead, BufReader};
use regex::Regex;

use crate::lexer;
use crate::token;
use crate::semantics::SemanticError;

#[derive(Clone)]
struct Line {
    number: usize,
    start: usize,
    end: usize,
    contents: String
}

#[derive(Debug)]
pub(crate) enum SourceError {

    UnableToOpenFile,
    LineReadError,
}

pub(crate) struct Source {
    
    pub(crate) current_file: String,               // Raw data
    process_able: String,               // Process able data
    lines: Vec<Line>,                   // Lines of source
}

impl Source {

    pub fn get_source(&self) -> &String {
       &self.process_able
    }


    pub(crate) fn from_string(source: &String) -> Self {

        let mut v : Vec<Line> = Vec::new();

        v.push(
            Line{
                number: 1,
                start:  0,
                end:    source.len(),
                contents: source.clone()
            }
        );

        Self {
            process_able: source.clone(),
            lines: v,
            current_file: String::from("Raw Source")
        }
    }

    pub(crate) fn from_file(file: &String) -> Result<Self, SourceError> {

        let file_in = match File::open(file) {
            Ok(o) => o,
            Err(_) => return Err(SourceError::UnableToOpenFile)
        };
    
        // Reader
        let reader = BufReader::new(file_in);
    
        // Regex things for detecting things
        let comment_line = Regex::new(r"\-{2}[\W+\w+]*").unwrap();
    
        let mut conjoined_input : String = String::new();
    
        let mut line_vector : Vec<Line> = Vec::new();

        // Iter over lines and build things
        for (index, line) in reader.lines().enumerate() {
    
            let line = match line {
                Ok(l) => l,
                Err(_) => return Err(SourceError::LineReadError)
            };
    
            // If there is a comment on the line, then we want to ignore it
            if comment_line.is_match(line.as_str()) {
                conjoined_input = conjoined_input + " ";
                continue;
            }
    
            // Store line info for errors
            line_vector.push(Line{
                number: index+1,        // We want to 1-index not 0-index
                start:  conjoined_input.len(),
                end:    conjoined_input.len() + line.len(),
                contents: line.clone().trim_start().trim_end().to_string()
            });

            // Concat for processable
            conjoined_input = conjoined_input + " " + line.as_str();
        }

        Ok(Self{
            process_able: conjoined_input,
            lines: line_vector,
            current_file: file.clone()
        })
    }

    /// Show the line something was on
    fn show_line_item(&self, start: usize, end: usize) {
        for line in self.lines.iter() {
            if start >= line.start &&  end-1 <= line.end {
                println!("Line ({}):\n\t{}", line.number, line.contents);
                println!("\t{}", self.make_arrow(true, line.start, start-line.start, line.contents.len()));
                return;
            }
        }
    }

    /// Show line
    fn show_line(&self, start: usize, end: usize) {
        for line in self.lines.iter() {

           // println!("Looking for ({}, {}) >>> ({}, {})", start, end, line.start, line.end);
            if start >= line.start &&  end-1 <= line.end {
                println!("Line ({}):\n\t{}", line.number, line.contents);
                return;
            }
        }
    }

    /// Show line number
    fn get_line_number(&self, start: usize, end: usize) -> usize {
        for line in self.lines.iter() {

           // println!("Looking for ({}, {}) >>> ({}, {})", start, end, line.start, line.end);
            if start >= line.start &&  end-1 <= line.end {
                return line.number;
            }
        }
        return 0;
    }

    /// Make an arrow on the screen
    fn make_arrow(&self, with_head: bool, start: usize, head: usize, end: usize) -> String {

        let mut output: String = String::new();
        for x in 0..end {
            if x == start && x == head {
                output.push('~');
            }
            if with_head && x == head {
                output.push('^')
            } else {
                output.push('~');
            }
        }
        return output;
    }

    /// Show the error that occurred with the source
    pub(crate) fn show_parser_error(&self, error: lalrpop_util::ParseError<usize, token::Tok, lexer::LexicalError>)  {

        println!("Source from ({})\n", self.current_file);

        match error {

            lalrpop_util::ParseError::InvalidToken { location } => {

                println!("Invalid token at {}", location);
                self.show_line_item(location, location);
            },
            lalrpop_util::ParseError::UnrecognizedEOF { location, expected } => {

                println!("Unrecognized EOF at {}", location);
                println!("\nExpected one of these :");
                print!("\t");
                for item in expected.iter() {
                    print!("'{}', ", item)
                }
            },
            lalrpop_util::ParseError::UnrecognizedToken { token, expected } => {
                
                //println!("Unrecognized token '{:?}'", token.1);
                //println!("Show source starting at : {}, and ending around {}", token.0, token.2);
                //println!("Lalrpop expected one of these : \n {:?}", expected);

                println!("Unrecognized token!");

                self.show_line_item(token.0, token.2);

                println!("\nExpected on of the following : ");
                print!("\t");
                for item in expected.iter() {
                    print!("'{}', ", item)
                }
                println!("\n");
            },
            lalrpop_util::ParseError::ExtraToken { token } => {
                
                println!("Extra token found : '{:?}'\n", token.1);
                self.show_line_item(token.0, token.2);
            },
            lalrpop_util::ParseError::User { error } => {

                println!("User-defined error : {:?}", error);
            }
        }
    }

    pub(crate) fn show_semantic_error(&self, semantic_error: SemanticError) {

        println!("Source from ({})\n", self.current_file);

        match semantic_error {

            SemanticError::StaticModuleContainsNoInit{ module, loc } => {
                println!("\nStatic module \"{}\" has no \"init\" method defined.\n", module);
                println!("\nThe module in question is declared on line {}\n", self.get_line_number(loc.start, loc.start));
            }

            SemanticError::StaticInitContainsParameters{ module, loc } => {
                println!("\nStatic module \"{}\" has an \"init\" method defined 'with' parameters. Static init methods should contain '0' parameters.\n", module);
                self.show_line(loc.start, loc.start);
                println!("");
            }

            SemanticError::DuplicateModuleName{ module, first_seen, duplicate_loc } => {
                println!("\nDuplicated module \"{}\" ", module);

                println!("\nFirst appeared here: ");
                self.show_line(first_seen.start, first_seen.start);

                println!("\nThen again here:");
                self.show_line(duplicate_loc.start, duplicate_loc.start);
            }

            SemanticError::MalformedModule { module, loc } => {
                println!("\nMalformed module \"{}\". Does it need to be static?", module);

                println!("\nDefinition: ");
                self.show_line(loc.start, loc.start);
            }
            SemanticError::NonStaticExternPresent{ module, loc } => {
                println!("\nModule \"{}\" has methods declared as 'extern' but it is not a static method", module);

                println!("\nDefinition: ");
                self.show_line(loc.start, loc.start);
            }
            SemanticError::ExternLacksDefinition { module, extern_def, loc: _ } => {
                println!("\nModule \"{}\" has method \"{}\" declared as an extern, but that method is not defined within the module.\n", module, extern_def);

            }
            SemanticError::DuplicateMethod { module, method, first_seen, duplicate_loc }  => {
                println!("\nDuplicated method \"{}\" in module \"{}\" ", method, module);

                println!("\nFirst appeared here: ");
                self.show_line(first_seen.start, first_seen.start);

                println!("\nThen again here:");
                self.show_line(duplicate_loc.start, duplicate_loc.start);
            }
            SemanticError::DuplicateUnits { first_seen, duplicate_loc, unit }  => {
                println!("\nDuplicated unit \"{}\" found in source.", unit );

                println!("\nFirst appeared here: {}", first_seen);

                println!("\nThen again here: {}\n", duplicate_loc);
            }
        }
    }
}