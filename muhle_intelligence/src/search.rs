use std::time;

use crate::game;
use crate::evaluation;
use crate::move_generation;
use crate::engine;

pub struct Search<'a> {
    nodes: Vec<game::SearchNode<'a>>,
    engine: &'a engine::Engine,
}

pub struct SearchContext {
    best_move: game::Move,
}

impl<'a> Search<'a> {
    pub fn new(engine: &'a engine::Engine) -> Self {
        Self {
            nodes: Vec::new(),
            engine,
        }
    }

    pub fn search(mut self, mut ctx: SearchContext, position: &game::Position) -> Result<Option<game::Move>, String> {
        let current_node = self.setup(position);

        let begin = time::Instant::now();
        let eval = ctx.minimax(4, 0, current_node);
        let end = time::Instant::now();

        // self.engine.info((end - begin).as_secs_f64(), eval)?;

        if ctx.best_move == game::Move::default() {
            return Ok(None);
        }

        Ok(Some(ctx.best_move))
    }

    fn setup(&mut self, position: &game::Position) -> &game::SearchNode<'a> {
        // self.nodes.push(game::SearchNode::from_position(position));

        // self.nodes.last().unwrap()

        todo!()
    }
}

impl SearchContext {
    pub fn new() -> Self {
        Self {
            best_move: game::Move::default(),
        }
    }

    fn minimax(
        &mut self,
        depth: u32,
        plies_from_root: u32,
        current_node: &game::SearchNode,
    ) -> evaluation::Eval {
        // if depth == 0 || game::is_game_over_winner_material(current_node) {
        //     return evaluation::static_evaluation(current_node);
        // }

        // let mut max_evaluation = evaluation::Eval::MIN;

        // let moves = move_generation::generate_moves(&current_node);

        // if moves.is_empty() {  // Game over
        //     return evaluation::static_evaluation(current_node);
        // }

        // for move_ in moves {
        //     let mut new_node = game::SearchNode::from_node(current_node);

        //     new_node.play_move(&move_);

        //     let evaluation = -self.minimax(depth - 1, plies_from_root + 1, &new_node);

        //     if evaluation > max_evaluation {
        //         max_evaluation = evaluation;

        //         if plies_from_root == 0 {
        //             self.best_move = move_;
        //         }
        //     }
        // }

        // max_evaluation

        todo!()
    }
}
