#![allow(unused)]
use netedit::{NetEdit, Protocol, proxy_connection};
use rand::{Rng};
use std::env;
use std::io::{self, Error, ErrorKind, Read, Write};
use std::net::{Shutdown, TcpStream};
use std::process;

const NEVER_GONNA_GIVE_YOU_UP_NEVER_GONNA_LET_YOU_DOWN_NEVER_GONNA_RUN_AROUND_AND_DESERT_YOU: &str =
    "https://www.youtube.com/watch?v=dQw4w9WgXcQ";

pub fn rickroll_proxy(mut incoming_stream: TcpStream) -> Result<(), String> {
    let mut rng = rand::thread_rng();

    if rng.gen::<f64>() < 1.0 {
        // Rick Roll
        println!("Get rick-rolled bruv");
        incoming_stream.write_all(
            format!("HTTP/1.1 301 Moved Permanently\r\nLocation: {}\r\n",
                NEVER_GONNA_GIVE_YOU_UP_NEVER_GONNA_LET_YOU_DOWN_NEVER_GONNA_RUN_AROUND_AND_DESERT_YOU
            ).as_bytes()
        ).map_err(|e| e.to_string())?;
        incoming_stream.shutdown(Shutdown::Both);
    } else {
        proxy_connection(incoming_stream);
    }

    Ok(())
}

fn main() -> Result<(), String> {
    let netedit = NetEdit::new(Protocol::TCP, 11110, rickroll_proxy);
    println!("Try it out on http://httpforever.com/ or http://neverssl.com");
    netedit.listen();

    Ok(())
}
