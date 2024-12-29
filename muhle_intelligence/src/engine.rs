use std::collections::HashMap;
use std::str::FromStr;

use crate::evaluation;
use crate::game;
use crate::search;
use crate::options;

pub struct Engine {
    options: HashMap<String, options::Option>,
    position: game::Position,
    previous_positions: Vec<game::Position>,
    moves_played: Vec<game::Move>,
}

impl Engine {
    pub fn new() -> Self {
        Self {
            options: HashMap::new(),
            position: game::Position::default(),
            previous_positions: Vec::new(),
            moves_played: Vec::new(),
        }
    }

    pub fn gbgp(&mut self) {

    }

    pub fn newgame(&mut self, position: Option<String>) -> Result<(), String> {
        // if let Some(position) = position {
        //     self.game.position = game::Position::from_str(&position)?;
        // } else {
        //     self.game.position = game::Position::default();
        // }

        // self.game.previous_positions.clear();
        // self.game.previous_positions.push(self.game.position.clone());

        // self.game.moves_played.clear();

        // Ok(())

        todo!()
    }

    pub fn move_(&mut self, move_: String) -> Result<(), String> {
        // if various::is_game_over(&self.game.position) {
        //     return Ok(());
        // }

        // let move_ = game::Move::from_str(&move_)?;

        // self.game.position.play_move(&move_);
        // self.game.moves_played.push(move_);
        // self.game.previous_positions.push(self.game.position.clone());

        // Ok(())

        todo!()
    }

    pub fn go(&mut self, no_play: bool) -> Result<(), String> {
        // let search = search::Search::new(&self);
        // let ctx = search::SearchContext::new();

        // let best_move = search.search(ctx, &self.game.position)?;  // FIXME returned default (invalid) move

        // if let Some(best_move) = &best_move {
        //     if !no_play {
        //         self.game.position.play_move(best_move);
        //         self.game.moves_played.push(best_move.clone());
        //         self.game.previous_positions.push(self.game.position.clone());
        //     }
        // }

        // self.bestmove(best_move.as_ref())?;

        // Ok(())

        todo!()
    }

    pub fn stop(&mut self) {

    }

    pub fn quit(&mut self) {

    }

    pub fn ready(&self) -> Result<(), String> {
        // let buffer = String::from("ready\n");

        // (self.message_writer)(buffer)?;

        // Ok(())

        todo!()
    }

    pub fn info(&self, time: f64, eval: evaluation::Eval) -> Result<(), String> {
        // let buffer = format!("info time {} eval {}\n", time, eval);

        // (self.message_writer)(buffer)?;

        // Ok(())

        todo!()
    }

    fn bestmove(&self, move_: Option<&game::Move>) -> Result<(), String> {
        // let buffer = if let Some(move_) = move_ {
        //     format!("bestmove {}\n", move_.to_string())
        // } else {
        //     String::from("bestmove none\n")
        // };

        // (self.message_writer)(buffer)?;

        // Ok(())

        todo!()
    }
}
