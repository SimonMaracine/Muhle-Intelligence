use crate::game;
use crate::search;

pub type Eval = i32;

pub fn static_evaluation(node: &search::SearchNode) -> Eval {
    let mut evaluation: Eval = 0;

    for piece in node.board {
        match piece {
            game::Piece::White => {
                evaluation += 1;
            }
            game::Piece::Black => {
                evaluation -= 1;
            }
            game::Piece::None => (),
        }
    }

    let perspective = if node.player == game::Player::White { 1 } else { -1 };

    evaluation * perspective
}
