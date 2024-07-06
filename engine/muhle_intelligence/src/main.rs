mod main_loop;
mod commands;
mod engine;
mod game;
mod search;
mod evaluation;
mod move_generation;
mod various;

use std::process::ExitCode;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new();
    let result = main_loop::main_loop(&mut engine);

    if let Err(err) = result {
        eprintln!("An error occurred: {}", err);
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}
