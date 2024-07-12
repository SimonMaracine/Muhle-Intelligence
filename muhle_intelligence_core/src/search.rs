use crate::game;
use crate::evaluation;
use crate::move_generation;
use crate::various;

pub struct SearchNode<'a> {
    pub board: game::Board,
    pub player: game::Player,
    pub plies: u32,
    pub plies_without_advancement: u32,
    // TODO repetition

    previous: Option<&'a SearchNode<'a>>,
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

    pub fn search(mut self, mut ctx: SearchContext, position: &game::Position) -> Option<game::Move> {
        let current_node = self.setup(position);

        let evalation = ctx.minimax(
            4,
            0,
            current_node,
        );

        if ctx.best_move == game::Move::default() {
            return None;
        }

        Some(ctx.best_move)
    }

    fn setup(&mut self, position: &game::Position) -> &SearchNode<'a> {
        self.nodes.push(SearchNode::from_position(position));

        self.nodes.last().unwrap()
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
        current_node: &SearchNode,
    ) -> evaluation::Eval {
        if depth == 0 || various::is_game_over_winner_material(current_node) {
            return evaluation::static_evaluation(current_node);
        }

        let mut max_evaluation = evaluation::Eval::MIN;

        let moves = move_generation::generate_moves(&current_node);

        if moves.is_empty() {  // Game over
            return evaluation::static_evaluation(current_node);
        }

        for move_ in moves {
            let mut new_node = SearchNode::from_node(current_node);

            new_node.play_move(&move_);

            let evaluation = -self.minimax(depth - 1, plies_from_root + 1, &new_node);

            if evaluation > max_evaluation {
                max_evaluation = evaluation;

                if plies_from_root == 0 {
                    self.best_move = move_;
                }
            }
        }

        max_evaluation
    }
}

impl<'a> SearchNode<'a> {
    pub fn from_position(position: &game::Position) -> Self {
        Self {
            board: position.board,
            player: position.player,
            plies: position.plies,
            plies_without_advancement: position.plies_without_advancement,
            previous: None,
        }
    }

    fn from_node(node: &'a SearchNode) -> Self {
        Self {
            board: node.board,
            player: node.player,
            plies: node.plies,
            plies_without_advancement: node.plies_without_advancement,
            previous: Some(node),
        }
    }

    fn play_move(&mut self, move_: &game::Move) {
        match *move_ {
            game::Move::Place { place_index } => {
                assert!(self.board[place_index as usize] == game::Piece::None);

                self.board[place_index as usize] = various::player_piece(self.player);

                self.plies_without_advancement += 1;
            }
            game::Move::PlaceTake { place_index, take_index } => {
                assert!(self.board[place_index as usize] == game::Piece::None);
                assert!(self.board[take_index as usize] != game::Piece::None);

                self.board[place_index as usize] = various::player_piece(self.player);
                self.board[take_index as usize] = game::Piece::None;

                self.plies_without_advancement = 0;
                self.previous = None;
            }
            game::Move::Move { source_index, destination_index } => {
                assert!(self.board[source_index as usize] != game::Piece::None);
                assert!(self.board[destination_index as usize] == game::Piece::None);

                self.board.swap(source_index as usize, destination_index as usize);

                self.plies_without_advancement += 1;
            }
            game::Move::MoveTake { source_index, destination_index, take_index } => {
                assert!(self.board[source_index as usize] != game::Piece::None);
                assert!(self.board[destination_index as usize] == game::Piece::None);
                assert!(self.board[take_index as usize] != game::Piece::None);

                self.board.swap(source_index as usize, destination_index as usize);
                self.board[take_index as usize] = game::Piece::None;

                self.plies_without_advancement = 0;
                self.previous = None;
            }
        }

        self.player = various::opponent(self.player);
        self.plies += 1;
    }
}
