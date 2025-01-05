mod commands;
mod engine;
mod evaluation;
mod game;
mod messages;
mod move_generation;
mod options;
mod think;

use std::io;
use std::fs;
use std::io::Write;
use std::process::ExitCode;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new();
    let mut log_file: Option<fs::File> = None;

    let result = main_loop(&mut engine, &mut log_file);

    if let Err(err) = result {
        if engine.is_debug_mode() {
            let _ = write_to_log_file(&mut log_file, format!("[Critical error]  {}\n", err));
        }
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}

fn main_loop(engine: &mut engine::Engine, log_file: &mut Option<fs::File>) -> Result<(), String> {
    loop {
        let tokens = match read_from_stdin() {
            Ok(input) => tokenize_input(input),
            Err(err) => return Err(format!("Could not read input: {}", err)),
        };

        if tokens.is_empty() {
            continue;
        }

        if engine.is_debug_mode() {
            let _ = write_to_log_file(log_file, format!("[Command]  {:?}\n", tokens));
        }

        if tokens[0] == "quit" {
            commands::quit(engine, tokens);
            return Ok(());
        }

        if let Err(err) = execute_command(engine, tokens) {
            if engine.is_debug_mode() {
                let _ = write_to_log_file(log_file, format!("[Error]  {}\n", err));
            }
        }
    }
}

fn write_to_log_file(log_file: &mut Option<fs::File>, buffer: String) -> Result<(), io::Error> {
    if let None = log_file {
        let mut file = fs::OpenOptions::new().append(true).create(true).open("muhle_intelligence.log")?;
        file.write_all(format!("--- Log Begin ---\n").as_bytes())?;  // TODO time
        *log_file = Some(file);
    }

    let log_file = log_file.as_mut().expect("The log file should be open");

    log_file.write_all(buffer.as_bytes())?;

    Ok(())
}

fn read_from_stdin() -> Result<String, io::Error> {
    let mut buffer = String::new();
    io::stdin().read_line(&mut buffer)?;
    Ok(buffer)
}

fn execute_command(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    assert!(!tokens.is_empty());

    let command = tokens[0].as_str();

    if !engine.is_gbgp_mode() {
        if command == "gbgp" {
            commands::gbgp(engine, tokens)?;
        }
    } else {
        match command {
            "gbgp" => {
                commands::gbgp(engine, tokens)?;
            }
            "debug" => {
                commands::debug(engine, tokens)?;
            }
            "isready" => {
                commands::isready(engine, tokens)?;
            }
            "setoption" => {
                commands::setoption(engine, tokens)?;
            }
            "newgame" => {
                commands::newgame(engine, tokens);
            }
            "position" => {
                commands::position(engine, tokens)?;
            }
            "go" => {
                commands::go(engine, tokens)?;
            }
            "stop" => {
                commands::stop(engine, tokens);
            }
            "ponderhit" => {
                commands::ponderhit(engine, tokens);
            }
            _ => return Err(format!("Invalid command: `{}`", command))
        }
    }

    Ok(())
}

fn tokenize_input(input: String) -> Vec<String> {
    input.split([' ', '\t']).filter(|token| {
        !token.trim().is_empty()
    }).map(|token| {
        String::from(token.trim())
    }).collect::<Vec<_>>()
}

#[cfg(test)]
mod test {
    use std::str::FromStr;

    use super::game;

    #[test]
    fn position() {
        assert_eq!(
            game::Position::default(),
            game::Position::from_str("w:w:b:1").unwrap(),
        );

        let position = game::Position {
            board: [
                game::Node::Black,
                game::Node::Black,
                game::Node::White,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::Empty,
                game::Node::White,
                game::Node::Black,
                game::Node::White,
            ],
            player: game::Player::Black,
            plies: 5,
        };

        assert_eq!(
            position,
            game::Position::from_str("b:wg7,a1,g1:ba7,d7,d1:3").unwrap(),
        );

        assert!(game::Position::from_str("pwemo3icm 80").is_err());
        assert!(game::Position::from_str("ofh3c:2bv73").is_err());
        assert!(game::Position::from_str("  w:w:b:0  ").is_err());
        assert!(game::Position::from_str("w:w:y9:b:0").is_err());
        assert!(game::Position::from_str("w:w:bj8:0").is_err());
        assert!(game::Position::from_str("a:w:b:0").is_err());
        assert!(game::Position::from_str("w:b:b:0").is_err());
    }
}
