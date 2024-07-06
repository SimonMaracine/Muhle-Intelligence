use std::collections::HashMap;

use crate::game;
use crate::search;

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

    }

    pub fn newgame(&mut self, position: Option<String>) {

    }

    pub fn r#move(&mut self, r#move: String) {

    }

    pub fn go(&mut self, no_play: bool) {
        let search = search::Search::new();
        let ctx = search::SearchContext::new();

        let best_move = search.search(ctx);

        self.game.position.play_move(&best_move);

        // TODO send bestmove message
    }

    pub fn stop(&mut self) {

    }

    pub fn quit(&mut self) {

    }
}
