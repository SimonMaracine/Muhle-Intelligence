use std::collections::HashMap;
use std::str::FromStr;

use crate::game;
use crate::search;
use crate::messages;

pub enum Parameter {
    Int(i32),
}

pub struct Engine {
    parameters: HashMap<String, Parameter>,
    game: Game,
}

struct Game {
    position: game::Position,
    previous_positions: Vec<game::Position>,
    moves_played: Vec<game::Move>,
}

impl Engine {
    pub fn new() -> Self {
        Self {
            parameters: HashMap::new(),
            game: Game {
                position: game::Position::default(),
                previous_positions: Vec::new(),
                moves_played: Vec::new(),
            },
        }
    }

    pub fn init(&mut self) {
        todo!()
    }

    pub fn newgame(&mut self, position: Option<String>) {
        // TODO parameter

        self.game.position = game::Position::default();
    }

    pub fn r#move(&mut self, r#move: String) -> Result<(), String> {
        let r#move = game::Move::from_str(&r#move)?;

        self.game.position.play_move(&r#move);

        Ok(())
    }

    pub fn go(&mut self, no_play: bool) {
        // TODO parameter

        let search = search::Search::new();
        let ctx = search::SearchContext::new();

        let best_move = search.search(ctx);

        self.game.position.play_move(&best_move);

        messages::bestmove(best_move);  // TODO
    }

    pub fn stop(&mut self) {
        todo!()
    }

    pub fn quit(&mut self) {
        todo!()
    }
}
