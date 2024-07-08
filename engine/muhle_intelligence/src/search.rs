use crate::game;
use crate::evaluation as eval;
use crate::move_generation;
use crate::various;

pub struct SearchNode<'a> {
    pub board: game::Board,
    pub player: game::Player,
    pub plies: u32,
    pub plies_without_advancement: u32,
    // TODO repetition

    pub previous: Option<&'a SearchNode<'a>>,
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
    pub fn play_move(&mut self, r#move: &game::Move) {
        match r#move {
            game::Move::Place { place_index } => {
                self.board[*place_index as usize] = various::player_piece(self.player);

                self.plies_without_advancement += 1;
            }
            game::Move::PlaceTake { place_index, take_index } => {
                self.board[*place_index as usize] = various::player_piece(self.player);
                self.board[*take_index as usize] = game::Piece::None;

                self.plies_without_advancement = 0;
                self.previous = None;
            }
            game::Move::Move { source_index, destination_index } => {
                self.board.swap(*source_index as usize, *destination_index as usize);

                self.plies_without_advancement += 1;
            }
            game::Move::MoveTake { source_index, destination_index, take_index } => {
                self.board.swap(*source_index as usize, *destination_index as usize);
                self.board[*take_index as usize] = game::Piece::None;

                self.plies_without_advancement = 0;
                self.previous = None;
            }
        }

        self.player = various::opponent(self.player);
        self.plies += 1;
    }
}
