use std::io::{self, Write};
use std::process::ExitCode;

use muhle_intelligence_core::engine;
use muhle_intelligence_core::commands;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new(write_to_stdout);
    let result = main_loop(&mut engine);

    if let Err(err) = result {
        eprintln!("An error occurred: {}", err);
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}

fn main_loop(engine: &mut engine::Engine) -> Result<(), String> {
    loop {
        let input = read_from_stdin();

        if let Err(err) = input {
            return Err(format!("Could not read input: {}", err));
        }

        let tokens = commands::tokenize_command_input(input.unwrap());

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
        return Err(format!("Could not write to stdout: {}", err.to_string()));
    }

    Ok(())
}
