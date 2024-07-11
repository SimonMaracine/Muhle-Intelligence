use crate::game;
use crate::engine;

pub fn bestmove(engine: &engine::Engine, move_: Option<&game::Move>) -> Result<(), String> {
    let buffer = if let Some(move_) = move_ {
        format!("bestmove {}\n", move_.to_string())
    } else {
        String::from("bestmove none\n")
    };

    engine.get_message_writer()(buffer)?;

    Ok(())
}
