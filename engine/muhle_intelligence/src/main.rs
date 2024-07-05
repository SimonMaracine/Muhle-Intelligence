mod main_loop;
mod commands;

use std::process::ExitCode;

fn main() -> ExitCode {
    let result = main_loop::main_loop();

    if let Err(err) = result {
        eprintln!("An error occurred: {}", err);
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}
