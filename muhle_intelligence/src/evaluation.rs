use crate::game;

pub type Eval = i32;

pub const WINDOW_MAX: Eval = 1000;
pub const WINDOW_MIN: Eval = -MAX;

pub const MAX: Eval = WINDOW_MAX - 1;
pub const MIN: Eval = WINDOW_MIN + 1;

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
