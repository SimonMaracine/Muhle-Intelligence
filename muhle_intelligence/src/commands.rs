use std::str::FromStr;

use crate::engine;
use crate::game;

const START_POSITION: &str = "w:w:b:1";

pub fn gbgp(engine: &mut engine::Engine, _tokens: Vec<String>) -> Result<(), String> {
    engine.gbgp()
}

pub fn debug(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    if let Some(active) = tokens.get(1) {
        match active.as_str() {
            "on" => engine.debug(true),
            "off" => engine.debug(false),
            invalid => return Err(format!("Invalid token after `debug`: `{}`", invalid)),
        }
    } else {
        return Err(String::from("Expected token after `debug`"));
    }

    Ok(())
}

pub fn isready(engine: &mut engine::Engine, _tokens: Vec<String>) -> Result<(), String> {
    engine.isready()
}

pub fn setoption(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {  // FIXME multi-token
    let name = if let Some(index) = tokens.iter().position(|token| token.as_str() == "name") {
        if let Some(name) = tokens.get(index + 1) {
            name
        } else {
            return Err(String::from("Expected token after `name`"));
        }
    } else {
        return Err(String::from("Expected token after `setoption`"));
    };

    let value = if let Some(index) = tokens.iter().position(|token| token.as_str() == "value") {
        if let Some(value) = tokens.get(index + 1) {
            Some(value)
        } else {
            return Err(String::from("Expected token after `value`"));
        }
    } else {
        None
    };

    engine.setoption(name, value)
}

pub fn newgame(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.newgame();
}

pub fn position(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let position = if let Some(pos) = tokens.get(1) {
        match pos.as_str() {
            "pos" => {
                if let Some(pos) = tokens.get(2) {
                    game::Position::from_str(pos)?
                } else {
                    return Err(String::from("Expected token after `pos`"));
                }
            }
            "startpos" => game::Position::from_str(START_POSITION)?,
            invalid => return Err(format!("Invalid token after `position`: `{}`", invalid)),
        }
    } else {
        return Err(String::from("Expected token after `position`"));
    };

    let moves = if let Some(index) = tokens.iter().position(|token| token.as_str() == "moves") {
        let mut moves = Vec::new();

        for move_ in tokens.iter().skip(index + 1) {
            moves.push(game::Move::from_str(move_)?);
        }

        Some(moves)
    } else {
        None
    };

    engine.position(position, moves);

    Ok(())
}

pub fn go(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let ponder = if let Some(_) = tokens.iter().find(|token| token.as_str() == "ponder") {
        true
    } else {
        false
    };

    let wtime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "wtime") {
        if let Some(wtime) = tokens.get(index + 1) {
            Some(wtime.parse::<u32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `wtime`"));
        }
    } else {
        None
    };

    let btime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "btime") {
        if let Some(btime) = tokens.get(index + 1) {
            Some(btime.parse::<u32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `btime`"));
        }
    } else {
        None
    };

    let depth = if let Some(index) = tokens.iter().position(|token| token.as_str() == "depth") {
        if let Some(depth) = tokens.get(index + 1) {
            Some(depth.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `depth`"));
        }
    } else {
        None
    };

    let movetime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "movetime") {
        if let Some(movetime) = tokens.get(index + 1) {
            Some(movetime.parse::<u32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `movetime`"));
        }
    } else {
        None
    };

    engine.go(ponder, wtime, btime, depth, movetime)?;

    Ok(())
}

pub fn stop(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.stop();
}

pub fn ponderhit(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.ponderhit();
}

pub fn quit(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.quit();
}
