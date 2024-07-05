use std::{io, str::FromStr};

use crate::commands;

pub fn main_loop() -> Result<(), String> {
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
            commands::quit();
            return Ok(());
        }

        if let Err(err) = execute_command(tokens) {
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
    input.split(" ").filter(|token| {
        !token.trim().is_empty()
    }).map(|token| {
        let string = String::from_str(token.trim());

        match string {
            Ok(string) => string,
            Err(_) => String::from("__error_token__")
        }
    }).collect::<Vec<_>>()
}

fn execute_command(tokens: Vec<String>) -> Result<(), String> {
    let command = tokens[0].as_str();

    match command {
        "init" => {
            commands::init();
        }
        _ => return Err(format!("Invalid command: `{}`", command))
    }

    Ok(())
}
