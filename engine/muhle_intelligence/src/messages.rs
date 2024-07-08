use std::io::{self, Write};

use crate::game;

pub fn bestmove(r#move: game::Move) -> Result<(), String> {
    let buffer = String::from("bestmove ") + &r#move.to_string();

    write_to_stdout(buffer)?;

    Ok(())
}

fn write_to_stdout(buffer: String) -> Result<(), String> {
    let mut lock = io::stdout().lock();

    if let Err(err) = lock.write_all(buffer.as_bytes()) {
        return Err(err.to_string());
    }

    Ok(())
}
