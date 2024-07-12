use crate::engine;

pub fn execute_command(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let command = tokens[0].as_str();

    match command {
        "init" => {
            init(engine, tokens);
        }
        "newgame" => {
            newgame(engine, tokens)?;
        }
        "move" => {
            move_(engine, tokens)?;
        }
        "go" => {
            go(engine, tokens)?;
        }
        "stop" => {
            stop(engine, tokens);
        }
        _ => return Err(format!("Invalid command: `{}`", command))
    }

    Ok(())
}

pub fn tokenize_command_input(input: String) -> Vec<String> {
    input.split([' ', '\t']).filter(|token| {
        !token.trim().is_empty()
    }).map(|token| {
        String::from(token.trim())
    }).collect::<Vec<_>>()
}

pub fn quit(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.quit();
}

fn init(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.init();
}

fn newgame(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let position = tokens.get(1);

    engine.newgame(position.cloned())?;

    Ok(())
}

fn move_(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let move_ = tokens.get(1);

    if let None = move_ {
        return Err(String::from("Expected second token to be a move string"));
    }

    engine.move_(move_.unwrap().to_owned())?;

    Ok(())
}

fn go(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let noplay = tokens.get(1);
    let noplay = noplay.map_or(false, |noplay| { noplay == "noplay" });

    engine.go(noplay)?;

    Ok(())
}

fn stop(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.stop();
}
