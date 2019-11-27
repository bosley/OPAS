extern crate tui;
extern crate hex;
extern crate nexus;

pub const APPLICATION_NAME: &str = "Jewel";
pub const VERSION:          &str = "0.1.0";
pub const AUTHOR:           &str = "Josh A. Bosley";
pub const ABOUT:            &str = "Command line utility to generate and interact with DC networks";

mod configure;
pub mod run;
pub use configure::peer as peer;
pub use configure::network as network;
