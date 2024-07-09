use crate::game;
use crate::engine;

pub fn bestmove(engine: &engine::Engine, move_: game::Move) -> Result<(), String> {
    let buffer = format!("bestmove {}\n", move_.to_string());

    engine.get_message_writer()(buffer)?;

    Ok(())
}
