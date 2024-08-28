use netedit::{NetEdit, Protocol, proxy_connection};
use std::env;

fn err(message: &str) -> Result<(), String> {
    // Err(Error::other(message.to_string()))
    Err(message.to_string())
}

/// netedit <protocol> <port> <dst-host:dst-port>
fn main() -> Result<(), String> {
    let args: Vec<String> = env::args().collect();
    
    if args.len() != 3 {
        return err("Usage: netedit <tcp|udp> <port>");
    }

    let protocol = match args[1].to_lowercase().as_str() {
        "tcp" => Protocol::TCP,
        "udp" => Protocol::UDP,
        _ => return Err("Protocol must be either 'tcp' or 'udp'".to_string()),
    };

    let port = args[2].parse::<u16>()
        .map_err(|_| "Port must be a valid number".to_string())?;

    let netedit = NetEdit::new(protocol, port, proxy_connection);
    netedit.listen()
}
