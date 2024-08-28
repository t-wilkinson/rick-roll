use std::io::{self, Read, Write};
use std::net::{Shutdown, TcpListener, TcpStream};
use std::process;
use std::thread::{self, spawn};

pub enum Protocol {
    TCP,
    UDP,
}

type Handler = fn(TcpStream) -> Result<(), String>;

pub struct NetEdit {
    pub protocol: Protocol,
    pub port: u16,
    pub handler: Handler,
}

impl NetEdit {
    pub fn new(protocol: Protocol, port: u16, handler: Handler) -> Self {
        Self {
            protocol,
            port,
            handler,
        }
    }

    pub fn listen(&self) -> Result<(), String> {
        match self.protocol {
            Protocol::TCP => self.tcp_listener(),
            Protocol::UDP => Err("UDP not implemented yet".to_string()),
        }
    }

    fn tcp_listener(&self) -> Result<(), String> {
        let listener = TcpListener::bind(format!("127.0.0.1:{}", &self.port)).map_err(|e| {
            format!(
                "Could not establish tcp listener to 127.0.0.1:{}: {}",
                &self.port, e
            )
        })?;

        println!("Running on port {} with PID {}", self.port, process::id());

        for stream in listener.incoming() {
            let stream = stream.map_err(|e| format!("Failed to accept connection: {}", e))?;
            let handler = self.handler;
            spawn(move || handler(stream).unwrap_or_else(|e| eprintln!("Connection error: {}", e)));
        }

        Ok(())
    }
}

pub fn proxy_connection(mut incoming_stream: TcpStream) -> Result<(), String> {
    let dst_addr = extract_dst_addr(&mut incoming_stream)?;

    let mut incoming_write = incoming_stream;
    let mut outgoing_write = TcpStream::connect(dst_addr.clone())
        .map_err(|e| format!("Could not establish connection to {}: {}", dst_addr, e))?;

    let mut incoming_read = incoming_write.try_clone().map_err(|e| e.to_string())?;
    let mut outgoing_read = outgoing_write.try_clone().map_err(|e| e.to_string())?;

    let incoming_to_outgoing = thread::spawn(move || io::copy(&mut incoming_read, &mut outgoing_write));
    let outgoing_to_incoming = thread::spawn(move || io::copy(&mut outgoing_read, &mut incoming_write));

    // let forward = spawn(move || proxy::pipe(&mut incoming_stream, &mut outgoing_stream));
    // let backward =
    //     spawn(move || proxy::pipe(&mut outgoing_stream_clone, &mut incoming_stream_clone));

    let _ = incoming_to_outgoing.join().map_err(|e| format!("incoming_to_outgoing thread failed: {:?}", e))?;
    let _ = outgoing_to_incoming.join().map_err(|e| format!("outgoing_to_incoming thread failed: {:?}", e))?;

    Ok(())
}

fn extract_dst_addr(stream: &mut TcpStream) -> Result<String, String> {
    let mut buffer = [0; 1024];
    let n = stream
        .peek(&mut buffer)
        .map_err(|e| format!("Failed to read from stream: {}", e))?;

    let request = String::from_utf8_lossy(&buffer[..n]);
    if let Some(host_line) = request.lines().find(|line| line.starts_with("Host: ")) {
        let host = host_line[6..].trim();
        if host.contains(':') {
            Ok(host.to_string())
        } else {
            Err("Invalid Host header".to_string())
        }
    } else {
        Err("Could not parse destination address".to_string())
    }
}


const BUFFER_SIZE: usize = 128;

/// Pipe incoming TcpStream to outgoing TcpStream
pub fn pipe(incoming: &mut TcpStream, outgoing: &mut TcpStream) -> Result<(), String> {
    let mut buffer = [0; BUFFER_SIZE];
    loop {
        match incoming.read(&mut buffer) {
            Ok(bytes_read) => {
                // Done reading incoming stream
                if bytes_read == 0 || bytes_read < BUFFER_SIZE {
                    outgoing
                        .shutdown(Shutdown::Both)
                        .map_err(|e| format!("Error shutting down: {}", e))?;
                    break;
                }

                if outgoing.write(&buffer[..bytes_read]).is_ok() {
                    outgoing
                        .flush()
                        .map_err(|e| format!("Error writing buffer: {}", e))?;
                }
            }
            Err(e) => return Err(format!("Could not read data: {}", e)),
        }
    }

    Ok(())
}
