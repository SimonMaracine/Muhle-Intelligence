use std::str::FromStr;

use crate::engine;
use crate::game;

pub fn gbgp(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.gbgp();
}

pub fn debug(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    if let Some(active) = tokens.get(1) {
        match active.as_str() {
            "on" => engine.debug(true),
            "off" => engine.debug(false),
            invalid => return Err(format!("Invalid token: `{}`", invalid)),
        }
    } else {
        return Err(String::from("Expected token after `debug`"));
    }

    Ok(())
}

pub fn isready(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.isready();
}

pub fn setoption(engine: &mut engine::Engine, tokens: Vec<String>) {

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
            "startpos" => game::Position::from_str("")?,  // FIXME
            invalid => return Err(format!("Invalid token: `{}`", invalid)),
        }
    } else {
        return Err(String::from("Expected token after `position`"));
    };

    let moves = if let Some(index) = tokens.iter().position(|token| token.as_str() == "moves") {
        let mut moves = Vec::new();

        for move_ in tokens.iter().skip(index) {
            moves.push(game::Move::from_str(move_)?);
        }

        Some(moves)
    } else {
        None
    };

    engine.position(position, moves)
}

pub fn go(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let ponder = if let Some(_) = tokens.iter().find(|token| token.as_str() == "ponder") {
        true
    } else {
        false
    };

    let wtime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "wtime") {
        if let Some(wtime) = tokens.get(index + 1) {
            Some(wtime.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `wtime`"));
        }
    } else {
        None
    };

    let btime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "btime") {
        if let Some(btime) = tokens.get(index + 1) {
            Some(btime.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `btime`"));
        }
    } else {
        None
    };

    let maxdepth = if let Some(index) = tokens.iter().position(|token| token.as_str() == "maxdepth") {
        if let Some(maxdepth) = tokens.get(index + 1) {
            Some(maxdepth.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `maxdepth`"));
        }
    } else {
        None
    };

    let maxtime = if let Some(index) = tokens.iter().position(|token| token.as_str() == "maxtime") {
        if let Some(maxtime) = tokens.get(index + 1) {
            Some(maxtime.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?)
        } else {
            return Err(String::from("Expected token after `maxtime`"));
        }
    } else {
        None
    };

    engine.go(ponder, wtime, btime, maxdepth, maxtime);

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
