// use crate::engine;

// pub fn quit(engine: &mut engine::Engine, _tokens: Vec<String>) {
//     engine.quit();
// }

// fn init(engine: &mut engine::Engine, _tokens: Vec<String>) {
//     engine.init();
// }

// fn newgame(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
//     let position = tokens.get(1);

//     engine.newgame(position.cloned())?;

//     Ok(())
// }

// fn move_(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
//     let move_ = tokens.get(1);

//     let Some(move_) = move_ else {
//         return Err(String::from("Expected second token to be a move string"));
//     };

//     engine.move_(move_.to_owned())?;

//     Ok(())
// }

// fn go(engine: &mut engine::Engine, tokens: Vec<String>) -> Result<(), String> {
//     let noplay = tokens.get(1);
//     let noplay = noplay.map_or(false, |noplay| { noplay == "noplay" });

//     engine.go(noplay)?;

//     Ok(())
// }

// fn stop(engine: &mut engine::Engine, _tokens: Vec<String>) {
//     engine.stop();
// }
