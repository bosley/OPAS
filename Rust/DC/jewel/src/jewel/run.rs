use std::io;
use termion::event::Key;
use termion::input::MouseTerminal;
use termion::raw::IntoRawMode;
use termion::screen::AlternateScreen;
use tui::backend::TermionBackend;
use tui::layout::{Constraint, Corner, Direction, Layout};
use tui::style::{Color, Modifier, Style};
use tui::widgets::{Block, Borders, List, SelectableList, Text, Widget};
use tui::Terminal;

use termion::input::TermRead;
use std::sync::mpsc;
use std::thread;
use std::time::Duration;
use std::sync::{ Arc, Mutex };

extern crate chrono;
use chrono::Timelike;


const MAX_HEX_LENGTH:    usize = 50;
const HISTORY_LIMIT:     usize = 520;
const SLEEP_MILLIS:      Duration  = Duration::from_millis(100);

//  View App
//
pub struct ViewApp<'a> {

    pub network:       &'a nexus::system::Network,
    pub network_nodes: Vec<String>,
    pub network_data:  Vec<String>,
    pub should_quit:   bool,
    selected:          Option<usize>,
}

impl<'a> ViewApp <'a>{

    pub fn new(network: &'a nexus::system::Network) -> ViewApp<'a> {
        ViewApp {
            network,
            network_nodes: Vec::new(),
            network_data: Vec::new(),
            should_quit: false,
            selected: None
        }
    }
}

pub enum Event<I> {
    Input(I),
    Tick,
}

/// A small event handler that wrap termion input and tick events. Each event
/// type is handled in its own thread and returned to a common `Receiver`
#[allow(dead_code)]
pub struct Events {
    rx: mpsc::Receiver<Event<Key>>,
    input_handle: thread::JoinHandle<()>,
    tick_handle: thread::JoinHandle<()>,
}

#[derive(Debug, Clone, Copy)]
pub struct Config {
    pub exit_key: Key,
    pub tick_rate: Duration,
}

impl Default for Config {
    fn default() -> Config {
        Config {
            exit_key: Key::Char('q'),
            tick_rate: SLEEP_MILLIS,
        }
    }
}

impl Events {
    pub fn new() -> Events {
        Events::with_config(Config::default())
    }

    pub fn with_config(config: Config) -> Events {
        let (tx, rx) = mpsc::channel();
        let input_handle = {
            let tx = tx.clone();
            thread::spawn(move || {
                let stdin = io::stdin();
                for evt in stdin.keys() {
                    match evt {
                        Ok(key) => {
                            if let Err(_) = tx.send(Event::Input(key)) {
                                return;
                            }
                            if key == config.exit_key {
                                return;
                            }
                        }
                        Err(_) => {}
                    }
                }
            })
        };
        let tick_handle = {
            let tx = tx.clone();
            thread::spawn(move || {
                let tx = tx.clone();
                loop {
                    tx.send(Event::Tick).unwrap();
                    thread::sleep(config.tick_rate);
                }
            })
        };
        Events {
            rx,
            input_handle,
            tick_handle,
        }
    }

    pub fn next(&self) -> Result<Event<Key>, mpsc::RecvError> {
        self.rx.recv()
    }
}

//
/// Start running the cli as a node on a given network
//
pub fn nexus_network_viewer(connection: &String, proto: nexus::network::TransferProtocol, network_file: &String, remote_node: &String) -> Result<(), &'static str>{

    // Load the network
    let network = match nexus::system::network_from_file(network_file) {
        Some(net) => {
            net
        },
        None => {
            return Err("Unable to load network configuration");
        }
    };

    let known_remote = match nexus::system::local_from_file(&remote_node.to_string()) {
        Some(loc) => loc,
        None      => {
            return Err("Unable to load remote node from file");
        }
    };

    let mut jewel = nexus::system::generate_new_local(&"Jewel CLI".to_string(), &connection.to_string());
    jewel.id = "jewel-cli".to_string();

    jewel.add_peer(&nexus::system::Peer{
            id:         known_remote.id.to_string(),
            name:       known_remote.name.to_string(),
            connection: known_remote.connection.to_string(),
            last_seen:  chrono::Utc::now()
    });

    // Create the jewel node
    let jewel_arc = Arc::new(Mutex::new(jewel.clone()));

    let mut network_manager = nexus::network::get_network_manager(&proto, &jewel_arc, &network);
    
    println!("Nexus network viewer!");
    println!("Use ctrl+c to exit");
    
    if let Err(e) = network_manager.start() {
        panic!("Could not start network : {}", e);
    }

    if let Err(e) = network_manager.app_broadcast_registration() {
        eprintln!("Failed to broadcast registration request to known peers : {}", e);
    }
    
    if let Err(e) = network_manager.app_peer_synchronization() {
        eprintln!("Failed to broadcast peer synchronization request to known peers : {}", e);
    }

    let stdout = match io::stdout().into_raw_mode() {
        Ok(s)  => s,
        Err(_) => {
            return Err("Stdout error")
        }
    };

    let stdout = MouseTerminal::from(stdout);
    let stdout = AlternateScreen::from(stdout);
    let backend = TermionBackend::new(stdout);

    let mut terminal = match Terminal::new(backend){
        Ok(s)  => s,
        Err(_) => {
            return Err("terminal error")
        }
    };

    if let Err(_) = terminal.hide_cursor() {
        return Err("Error hiding cursor");
    }

    let events = Events::new();

    // App
    let mut app = ViewApp::new(&network);

    let mut last_sync = chrono::Utc::now();

    loop {

        //  Go through any received data and display with a time stamp
        //
        for app_data in network_manager.app_data_receiver.try_recv() {

            let display_id = match app_data.peer_id.split("-").next() {
                Some(id) => {
                    id
                },
                None => {
                    app_data.peer_id.as_str()
                }
            };

            if !app.network_nodes.contains(&app_data.peer_id) {
                app.network_nodes.push(app_data.peer_id.to_string());
            }

            if app.network_data.len() > HISTORY_LIMIT {
                app.network_data.drain(..20);
            }


            let now = chrono::Local::now();

            let mut hex_encoded = hex::encode_upper(app_data.raw_data).to_string();

            if hex_encoded.len() > MAX_HEX_LENGTH {
                hex_encoded = format!("{}...", hex_encoded).to_string();
            }

            app.network_data.push(format!("{:02}:{:02}:{:02} | From [{}] > {}",
                now.hour(),
                now.minute(),
                now.second(),
                display_id, 
                hex_encoded).to_string()
            );
        }

        // Draw app stuff
        if let Err(e) = terminal.draw(|mut f| {
            let chunks = Layout::default()
                .direction(Direction::Horizontal)
                .constraints([Constraint::Percentage(25), Constraint::Percentage(75)].as_ref())
                .split(f.size());
    
            let style = Style::default().fg(Color::White).bg(Color::Black);
            SelectableList::default()
                .block(Block::default().borders(Borders::ALL).title(&format!("Nodes on {}", app.network.network_name).to_string()))
                .items(&app.network_nodes)
                .select(app.selected)
                .style(style)
                .highlight_style(style.fg(Color::LightGreen).modifier(Modifier::BOLD))
                .highlight_symbol(">")
                .render(&mut f, chunks[0]);
            {
                let mut show_data = app.network_data.clone();
                show_data.reverse();

                let events = show_data.iter().map(|net_data| {
                    Text::styled(
                        net_data,
                        Style::default().fg(Color::White)
                    )
                });

                List::new(events)
                    .block(Block::default().borders(Borders::ALL).title("Broadcasted Network Data"))
                    .start_corner(Corner::TopLeft)
                    .render(&mut f, chunks[1]);
            }
        }) {
            eprintln!("Error drawing terminal : {}", e);
            return Err("Error drawing terminal");
        }

        let next_event = match events.next() {
            Ok(ne) => ne,
            Err(_) => continue
        };
    
        match next_event {
            Event::Input(input) => match input {
                Key::Char('q') => {
                    println!("Exiting");
                    return Ok(());
                }
                Key::Left => {
                    app.selected = None;
                }
                Key::Down => {
                    app.selected = if let Some(selected) = app.selected {
                        if selected >= app.network_nodes.len() - 1 {
                            Some(0)
                        } else {
                            Some(selected + 1)
                        }
                    } else {
                        Some(0)
                    }
                }
                Key::Up => {
                    app.selected = if let Some(selected) = app.selected {
                        if selected > 0 {
                            Some(selected - 1)
                        } else {
                            Some(app.network_nodes.len() - 1)
                        }
                    } else {
                        Some(0)
                    }
                }
                _ => {}
            },
            Event::Tick => {
                // chill
            }
        }
        
        if chrono::Utc::now().signed_duration_since(last_sync) > 
           chrono::Duration::minutes(1) {

            if let Err(e) = network_manager.app_broadcast_registration() {
                eprintln!("Failed to broadcast registration request to known peers : {}", e);
            }

            if let Err(e) = network_manager.app_peer_synchronization() {
                eprintln!("Failed to broadcast peer synchronization request to known peers : {}", e);
            }
            last_sync = chrono::Utc::now();
        }
    }
}
