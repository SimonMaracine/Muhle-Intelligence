mod commands;
mod engine;
mod evaluation;
mod game;
mod move_generation;
mod search;
mod various;

use std::io::{self, Write};
use std::process::ExitCode;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new(write_to_stdout);

    // First send a message for the parent process to know if we at least started
    if let Err(err) = engine.ready() {
        eprintln!("Could not send ready signal: {}", err);
        return ExitCode::from(1);
    }

    let result = main_loop(&mut engine);

    if let Err(err) = result {
        eprintln!("A critical error occurred: {}", err);
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}

fn main_loop(engine: &mut engine::Engine) -> Result<(), String> {
    loop {
        let tokens = match read_from_stdin() {
            Ok(input) => commands::tokenize_command_input(input),
            Err(err) => return Err(format!("Could not read input: {}", err)),
        };

        if tokens.is_empty() {
            continue;
        }

        if tokens[0] == "quit" {
            commands::quit(engine, tokens);
            return Ok(());
        }

        if let Err(err) = commands::execute_command(engine, tokens) {
            eprintln!("{}", err);
        }
    }
}

fn read_from_stdin() -> Result<String, io::Error> {
    let mut buffer = String::new();
    io::stdin().read_line(&mut buffer)?;
    Ok(buffer)
}

fn write_to_stdout(buffer: String) -> Result<(), String> {
    let mut lock = io::stdout().lock();

    if let Err(err) = lock.write_all(buffer.as_bytes()) {
        return Err(format!("Could not write to stdout: {}", err));
    }

    Ok(())
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
