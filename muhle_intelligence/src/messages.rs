use std::io::{self, Write};

use crate::game;
use crate::evaluation;
use crate::options;

pub enum Identifier {
    Name(String),
    Author(String),
}

pub enum Score {
    Eval(evaluation::Eval),
    Win(i32),
}

pub fn id(identifier: Identifier) -> Result<(), String> {
    let buffer = match identifier {
        Identifier::Name(name) => format!("id name {}\n", name),
        Identifier::Author(author) => format!("id author {}\n", author),
    };

    write_to_stdout(buffer)?;

    Ok(())
}

pub fn gbgpok() -> Result<(), String> {
    write_to_stdout(String::from("gbgpok\n"))
}

pub fn readyok() -> Result<(), String> {
    write_to_stdout(String::from("readyok\n"))
}

pub fn bestmove(best_move: Option<&game::Move>, ponder_move: Option<&game::Move>) -> Result<(), String> {
    let buffer = if let Some(best_move) = best_move {
        if let Some(ponder_move) = ponder_move {
            format!("bestmove {} ponder {}\n", best_move.to_string(), ponder_move.to_string())
        } else {
            format!("bestmove {}\n", best_move.to_string())
        }
    } else {
        String::from("bestmove none\n")
    };

    write_to_stdout(buffer)?;

    Ok(())
}

pub fn info(
    depth: Option<i32>,
    time: Option<i32>,
    nodes: Option<i32>,
    score: Option<Score>,
    currmove: Option<game::Move>,
    currmovenumber: Option<i32>,
    hashfull: Option<i32>,
    nps: Option<i32>,
    pv: Option<Vec<game::Move>>,
) -> Result<(), String> {
    let mut buffer = String::from("info");

    if let Some(depth) = depth {
        buffer += format!(" depth {}", depth).as_str();
    }

    if let Some(time) = time {
        buffer += format!(" time {}", time).as_str();
    }

    if let Some(nodes) = nodes {
        buffer += format!(" nodes {}", nodes).as_str();
    }

    if let Some(score) = score {
        match score {
            Score::Eval(eval) => buffer += format!(" score eval {}", eval).as_str(),
            Score::Win(moves) => buffer += format!(" score win {}", moves).as_str(),
        }
    }

    if let Some(currmove) = currmove {
        buffer += format!(" currmove {}", currmove.to_string()).as_str();
    }

    if let Some(currmovenumber) = currmovenumber {
        buffer += format!(" currmovenumber {}", currmovenumber).as_str();
    }

    if let Some(hashfull) = hashfull {
        buffer += format!(" hashfull {}", hashfull).as_str();
    }

    if let Some(nps) = nps {
        buffer += format!(" nps {}", nps).as_str();
    }

    if let Some(pv) = pv {
        buffer += " pv";

        for move_ in pv {
            buffer += format!(" {}", move_.to_string()).as_str();
        }
    }

    buffer.push('\n');

    write_to_stdout(buffer)?;

    Ok(())
}

pub fn option(name: &String, option: &options::Option) -> Result<(), String> {
    todo!()
}

fn write_to_stdout(buffer: String) -> Result<(), String> {
    let mut lock = io::stdout().lock();

    if let Err(err) = lock.write_all(buffer.as_bytes()) {
        return Err(format!("Could not write to stdout: {}", err));
    }

    if let Err(err) = lock.flush() {
        return Err(format!("Could not flush stdout: {}", err));
    }

    Ok(())
}
