use std::io::{self, Write};

use crate::game;

pub fn bestmove(move_: game::Move) -> Result<(), String> {
    let buffer = format!("bestmove {}\n", move_.to_string());

    write_to_stdout(buffer)?;

    Ok(())
}

fn write_to_stdout(buffer: String) -> Result<(), String> {
    let mut lock = io::stdout().lock();

    if let Err(err) = lock.write_all(buffer.as_bytes()) {
        return Err(format!("Could not write to stdout: {}", err.to_string()));
    }

    Ok(())
}
