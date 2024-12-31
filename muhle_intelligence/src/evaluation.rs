use crate::game;

pub type Eval = i32;

pub const MAX: Eval = 1000;
pub const MIN: Eval = -MAX;

pub const UNKNOWN: Eval = Eval::MIN;

pub fn static_evaluation(node: &game::SearchNode) -> Eval {
    let mut evaluation: Eval = 0;

    for node in node.position.position.board {
        match node {
            game::Node::White => evaluation += 1,
            game::Node::Black => evaluation -= 1,
            game::Node::Empty => (),
        }
    }

    evaluation
}

pub fn perspective(position: &game::Position) -> Eval {
    if position.player == game::Player::White { 1 } else { -1 }
}
