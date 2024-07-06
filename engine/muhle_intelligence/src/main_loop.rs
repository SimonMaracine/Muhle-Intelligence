use std::io;

use crate::commands;
use crate::engine;

pub fn main_loop(engine: &mut engine::Engine) -> Result<(), String> {
    loop {
        let input = read_input();

        if let Err(err) = input {
            return Err(format!("Could not read input: {}", err));
        }

        let tokens = tokenize_input(input.unwrap());

        if tokens.is_empty() {
            continue;
        }

        if tokens[0] == "quit" {
            commands::quit(engine, tokens);
            return Ok(());
        }

        if let Err(err) = execute_command(engine, tokens) {
            eprintln!("{}", err);
        }
    }
}

fn read_input() -> Result<String, io::Error> {
    let mut buffer = String::new();
    io::stdin().read_line(&mut buffer)?;
    Ok(buffer)
}

fn tokenize_input(input: String) -> Vec<String> {
    input.split([' ', '\t']).filter(|token| {
        !token.trim().is_empty()
    }).map(|token| {
        String::from(token.trim())
    }).collect::<Vec<_>>()
}

fn execute_command(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let command = tokens[0].as_str();

    match command {
        "init" => {
            commands::init(engine, tokens);
        }
        "newgame" => {
            commands::newgame(engine, tokens);
        }
        "move" => {
            commands::r#move(engine, tokens)?;
        }
        "go" => {
            commands::go(engine, tokens);
        }
        "stop" => {
            commands::stop(engine, tokens);
        }
        _ => return Err(format!("Invalid command: `{}`", command))
    }

    Ok(())
}
