use std::collections::HashMap;
use std::str::FromStr;

use crate::game;
use crate::search;
use crate::messages;
use crate::various;

pub enum Parameter {
    Int(i32),
}

type MessageWriter = fn(String) -> Result<(), String>;

pub struct Engine {
    parameters: HashMap<String, Parameter>,
    game: Game,
    message_writer: MessageWriter,
}

struct Game {
    position: game::Position,
    previous_positions: Vec<game::Position>,
    moves_played: Vec<game::Move>,
}

impl Engine {
    pub fn new(message_writer: MessageWriter) -> Self {
        Self {
            parameters: HashMap::new(),
            game: Game {
                position: game::Position::default(),
                previous_positions: Vec::new(),
                moves_played: Vec::new(),
            },
            message_writer,
        }
    }

    pub fn get_message_writer(&self) -> MessageWriter {
        self.message_writer
    }

    pub fn init(&mut self) {

    }

    pub fn newgame(&mut self, position: Option<String>) {
        // TODO parameter

        self.game.position = game::Position::default();
    }

    pub fn move_(&mut self, move_: String) -> Result<(), String> {
        if various::is_game_over(&self.game.position) {
            return Ok(());
        }

        eprintln!("Making move {}", move_);

        let move_ = game::Move::from_str(&move_)?;

        self.game.position.play_move(&move_);

        Ok(())
    }

    pub fn go(&mut self, no_play: bool) -> Result<(), String> {
        // TODO parameter

        eprintln!("Go");

        let search = search::Search::new();
        let ctx = search::SearchContext::new();

        let best_move = search.search(ctx, &self.game.position);  // FIXME returned default (invalid) move

        if let Some(best_move) = &best_move {
            eprintln!("Making best move {}", best_move.to_string());

            self.game.position.play_move(best_move);
        }

        messages::bestmove(self, best_move.as_ref())?;

        Ok(())
    }

    pub fn stop(&mut self) {

    }

    pub fn quit(&mut self) {

    }
}
