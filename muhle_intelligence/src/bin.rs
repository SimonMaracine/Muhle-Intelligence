use std::io::{self, Write};
use std::process::ExitCode;

use muhle_intelligence_core::engine;
use muhle_intelligence_core::commands;

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
