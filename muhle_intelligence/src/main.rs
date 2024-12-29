mod commands;
mod engine;
mod evaluation;
mod game;
mod messages;
mod move_generation;
mod options;
mod search;
mod think;

use std::io;
use std::process::ExitCode;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new();

    let result = main_loop(&mut engine);

    if let Err(err) = result {
        // eprintln!("A critical error occurred: {}", err);  // TODO write to log if debug
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}

fn main_loop(engine: &mut engine::Engine) -> Result<(), String> {
    loop {
        let tokens = match read_from_stdin() {
            Ok(input) => tokenize_command_input(input),
            Err(err) => return Err(format!("Could not read input: {}", err)),
        };

        if tokens.is_empty() {
            continue;
        }

        if tokens[0] == "quit" {
            commands::quit(engine, tokens);
            return Ok(());
        }

        if let Err(err) = execute_command(engine, tokens) {
            // eprintln!("{}", err);  // TODO write to log if debug
        }
    }
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
            commands::gbgp(engine, tokens);
        }
    } else {
        match command {
            "gbgp" => {
                commands::gbgp(engine, tokens);
            }
            "debug" => {
                commands::debug(engine, tokens);
            }
            "isready" => {
                commands::isready(engine, tokens);
            }
            "setoption" => {
                commands::setoption(engine, tokens);
            }
            "newgame" => {
                commands::newgame(engine, tokens);
            }
            "position" => {
                commands::position(engine, tokens);
            }
            "go" => {
                commands::go(engine, tokens);
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

fn tokenize_command_input(input: String) -> Vec<String> {
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
    fn parse_position() {
        assert_eq!(
            game::Position::default(),
            game::Position::from_str("w:;b:;w;0;0").unwrap(),
        );

        let position = game::Position {
            board: [
                game::Piece::Black,
                game::Piece::Black,
                game::Piece::White,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::White,
                game::Piece::Black,
                game::Piece::White,
            ],
            player: game::Player::Black,
            plies: 6,
            plies_without_advancement: 0,
        };

        assert_eq!(
            position,
            game::Position::from_str("w:g7,a1,g1;b:a7,d7,d1;b;6;0").unwrap(),
        );

        assert!(game::Position::from_str("ofh3c;2bv73").is_err());
        assert!(game::Position::from_str("  w:;b:;w;0;0  ").is_err());
        assert!(game::Position::from_str("w:y9;b:;w;0;0").is_err());
        assert!(game::Position::from_str("w:;b:j8;w;0;0").is_err());
        assert!(game::Position::from_str("w:;b:;a;0;0").is_err());
        assert!(game::Position::from_str("w:;b:;w;99999;0").is_err());
        assert!(game::Position::from_str("w:;b:;w;0;99999").is_err());
        assert!(game::Position::from_str("b:;b:;w;0;0").is_err());
    }
}
