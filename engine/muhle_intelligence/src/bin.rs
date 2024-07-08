mod main_loop;

use std::process::ExitCode;

use muhle_intelligence_core::engine;

fn main() -> ExitCode {
    let mut engine = engine::Engine::new();
    let result = main_loop::main_loop(&mut engine);

    if let Err(err) = result {
        eprintln!("An error occurred: {}", err);
        return ExitCode::from(1);
    }

    ExitCode::from(0)
}
