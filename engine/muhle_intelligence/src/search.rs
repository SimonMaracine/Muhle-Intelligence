use crate::game;
use crate::evaluation as eval;
use crate::move_generation;

pub struct SearchNode<'a> {
    pub board: game::Board,
    pub player: game::Player,
    pub plies: u32,
    pub plies_without_advancement: u32,

    pub previous: &'a SearchNode<'a>,
}

pub struct Search<'a> {
    nodes: Vec<SearchNode<'a>>,
}

pub struct SearchContext {
    best_move: game::Move,
}

impl<'a> Search<'a> {
    pub fn new() -> Self {
        Self {
            nodes: Vec::new(),
        }
    }

    pub fn search(mut self, mut ctx: SearchContext) -> game::Move {
        let current_node = self.nodes.last_mut().unwrap();

        let evalation = ctx.minimax(
            4,
            0,
            eval::Eval::max_value(),
            eval::Eval::min_value(),
            current_node,
        );

        ctx.best_move
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
        alpha: eval::Eval,
        beta: eval::Eval,
        current_node: &SearchNode,
    ) -> eval::Eval {
        if depth == 0 {
            return eval::static_evaluation(current_node);
        }

        if current_node.player == game::Player::Black {
            let min_evaluation = eval::Eval::max_value();

            let moves = move_generation::generate_moves(&current_node);
        } else {

        }



        todo!()
    }
}

impl<'a> SearchNode<'a> {
    pub fn play_move(r#move: &game::Move) {

    }
}
