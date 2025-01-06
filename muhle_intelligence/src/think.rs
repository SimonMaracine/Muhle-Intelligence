use std::time;

use crate::evaluation;
use crate::game;
use crate::messages;
use crate::move_generation;

pub struct ThinkContext {
    best_move: game::Move,
}

impl ThinkContext {
    pub fn new() -> Self {
        Self {
            best_move: game::Move::default(),
        }
    }
}

pub struct Think<'a> {
    nodes: Vec<game::SearchNode<'a>>,
}

impl<'a> Think<'a> {
    pub fn new() -> Self {
        Self {
            nodes: Vec::new(),
        }
    }

    pub fn think(
        mut self,
        position: &game::Position,
        moves: &Vec<game::Move>,
        mut ctx: ThinkContext,
    ) -> Option<game::Move> {
        let current_node = self.setup(position, moves);

        let begin = time::Instant::now();

        let eval = Self::alpha_beta(&mut ctx, 4, 0, evaluation::MIN, evaluation::MAX, current_node);

        let end = time::Instant::now();

        let _ = messages::info(
            Some(4),
            Some((end - begin).as_millis().try_into().expect("Should fit")),
            None,
            Some(messages::Score::Eval(eval)),
            None,
            None,
            None,
            None,
            None
        );

        if ctx.best_move == game::Move::default() {
            return None;
        }

        Some(ctx.best_move)
    }

    fn alpha_beta(
        ctx: &mut ThinkContext,
        depth: i32,
        depth_root: i32,
        mut alpha: evaluation::Eval,
        beta: evaluation::Eval,
        current_node: &game::SearchNode
    ) -> evaluation::Eval {
        if game::is_game_over_material(current_node) {  // Game over
            return evaluation::MIN;
        }

        let moves = move_generation::generate_moves(current_node);

        if moves.is_empty() {  // Game over
            return evaluation::MIN;
        }

        // TODO check fifty move rule + repetition

        if depth == 0 {
            return evaluation::static_evaluation(current_node) * evaluation::perspective(&current_node.position.position);
        }

        for move_ in moves {
            let mut new_node = game::SearchNode::from_node(current_node);
            new_node.play_move(&move_);

            let eval = -Self::alpha_beta(ctx, depth - 1, depth_root + 1, -beta, -alpha, &new_node);

            if eval >= beta {
                return beta;
            }

            if eval > alpha {
                alpha = eval;

                if depth_root == 0 {
                    ctx.best_move = move_;
                }
            }
        }

        return alpha;
    }

    fn setup(&mut self, position: &game::Position, moves: &Vec<game::Move>) -> &game::SearchNode<'a> {
        let mut game_position = game::GamePosition {
            position: position.clone(),
            plies_no_advancement: 0,
        };

        self.nodes.push(game::SearchNode::from_position(&game_position));

        for move_ in moves {
            game_position.play_move(move_);
            self.nodes.push(game::SearchNode::from_position(&game_position));
        }

        self.nodes.last().expect("There should be at least one node")
    }
}
