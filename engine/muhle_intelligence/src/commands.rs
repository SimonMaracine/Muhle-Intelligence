use crate::engine;

pub fn init(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.init();
}

pub fn newgame(engine: &mut engine::Engine, tokens: Vec<String>) {
    let position = tokens.get(1);
    let position = position.cloned();

    engine.newgame(position);
}

pub fn r#move(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
    let r#move = tokens.get(1);

    if let None = r#move {
        return Err(String::from("Expected second token to be a move string"));
    }

    engine.r#move(r#move.unwrap().to_owned());

    Ok(())
}

pub fn go(engine: &mut engine::Engine, tokens: Vec<String>) {
    let noplay = tokens.get(1);
    let noplay = noplay.map_or(false, |noplay| { noplay == "noplay" });

    engine.go(noplay);

    // TODO temp
    for token in &tokens {
        println!("`{}`", token);
    }
}

pub fn stop(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.stop();
}

pub fn quit(engine: &mut engine::Engine, _tokens: Vec<String>) {
    engine.quit();
}
