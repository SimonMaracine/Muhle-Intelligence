use crate::game;

pub type Eval = i32;

pub const WINDOW_MAX: Eval = 1000;
pub const WINDOW_MIN: Eval = -WINDOW_MAX;

pub const MAX: Eval = WINDOW_MAX - 1;
pub const MIN: Eval = WINDOW_MIN + 1;

pub const UNKNOWN: Eval = Eval::MIN;

const PLAYER_EVAL: Eval = 10;
const FREEDOM_EVAL: Eval = 1;
const ENDGAME_PLAYER_FREEDOM: i32 = 3 * 4;

pub fn static_evaluation(node: &game::SearchNode) -> Eval {
    let mut evaluation: Eval = 0;

    let mut white_pieces = 0;
    let mut black_pieces = 0;

    for node in node.position.position.board {
        match node {
            game::Node::White => {
                evaluation += PLAYER_EVAL;
                white_pieces += 1;
            }
            game::Node::Black => {
                evaluation -= PLAYER_EVAL;
                black_pieces += 1;
            }
            game::Node::Empty => (),
        }
    }

    let mut white_freedom = 0;
    let mut black_freedom = 0;

    for i in 0..24 {
        match node.position.position.board[i] {
            game::Node::White => white_freedom += piece_freedom(&node.position.position.board, i as game::Idx),
            game::Node::Black => black_freedom += piece_freedom(&node.position.position.board, i as game::Idx),
            game::Node::Empty => (),
        }
    }

    if node.position.position.plies >= 18 {
        if white_pieces == 3 {
            white_freedom = ENDGAME_PLAYER_FREEDOM;
        }

        if black_pieces == 3 {
            black_freedom = ENDGAME_PLAYER_FREEDOM;
        }
    }

    evaluation += white_freedom * FREEDOM_EVAL;
    evaluation -= black_freedom * FREEDOM_EVAL;

    evaluation
}

pub fn perspective(position: &game::Position) -> Eval {
    if position.player == game::Player::White { 1 } else { -1 }
}

fn piece_freedom(board: &game::Board, index: game::Idx) -> i32 {
    let mut result = 0;

    match index {
        0 => {
            if board[1] == game::Node::Empty {
                result += 1;
            }
            if board[9] == game::Node::Empty {
                result += 1;
            }
        }
        1 => {
            if board[0] == game::Node::Empty {
                result += 1;
            }
            if board[2] == game::Node::Empty {
                result += 1;
            }
            if board[4] == game::Node::Empty {
                result += 1;
            }
        }
        2 => {
            if board[1] == game::Node::Empty {
                result += 1;
            }
            if board[14] == game::Node::Empty {
                result += 1;
            }
        }
        3 => {
            if board[4] == game::Node::Empty {
                result += 1;
            }
            if board[10] == game::Node::Empty {
                result += 1;
            }
        }
        4 => {
            if board[1] == game::Node::Empty {
                result += 1;
            }
            if board[3] == game::Node::Empty {
                result += 1;
            }
            if board[5] == game::Node::Empty {
                result += 1;
            }
            if board[7] == game::Node::Empty {
                result += 1;
            }
        }
        5 => {
            if board[4] == game::Node::Empty {
                result += 1;
            }
            if board[13] == game::Node::Empty {
                result += 1;
            }
        }
        6 => {
            if board[7] == game::Node::Empty {
                result += 1;
            }
            if board[11] == game::Node::Empty {
                result += 1;
            }
        }
        7 => {
            if board[4] == game::Node::Empty {
                result += 1;
            }
            if board[6] == game::Node::Empty {
                result += 1;
            }
            if board[8] == game::Node::Empty {
                result += 1;
            }
        }
        8 => {
            if board[7] == game::Node::Empty {
                result += 1;
            }
            if board[12] == game::Node::Empty {
                result += 1;
            }
        }
        9 => {
            if board[0] == game::Node::Empty {
                result += 1;
            }
            if board[10] == game::Node::Empty {
                result += 1;
            }
            if board[21] == game::Node::Empty {
                result += 1;
            }
        }
        10 => {
            if board[3] == game::Node::Empty {
                result += 1;
            }
            if board[9] == game::Node::Empty {
                result += 1;
            }
            if board[11] == game::Node::Empty {
                result += 1;
            }
            if board[18] == game::Node::Empty {
                result += 1;
            }
        }
        11 => {
            if board[6] == game::Node::Empty {
                result += 1;
            }
            if board[10] == game::Node::Empty {
                result += 1;
            }
            if board[15] == game::Node::Empty {
                result += 1;
            }
        }
        12 => {
            if board[8] == game::Node::Empty {
                result += 1;
            }
            if board[13] == game::Node::Empty {
                result += 1;
            }
            if board[17] == game::Node::Empty {
                result += 1;
            }
        }
        13 => {
            if board[5] == game::Node::Empty {
                result += 1;
            }
            if board[12] == game::Node::Empty {
                result += 1;
            }
            if board[14] == game::Node::Empty {
                result += 1;
            }
            if board[20] == game::Node::Empty {
                result += 1;
            }
        }
        14 => {
            if board[2] == game::Node::Empty {
                result += 1;
            }
            if board[13] == game::Node::Empty {
                result += 1;
            }
            if board[23] == game::Node::Empty {
                result += 1;
            }
        }
        15 => {
            if board[11] == game::Node::Empty {
                result += 1;
            }
            if board[16] == game::Node::Empty {
                result += 1;
            }
        }
        16 => {
            if board[15] == game::Node::Empty {
                result += 1;
            }
            if board[17] == game::Node::Empty {
                result += 1;
            }
            if board[19] == game::Node::Empty {
                result += 1;
            }
        }
        17 => {
            if board[12] == game::Node::Empty {
                result += 1;
            }
            if board[16] == game::Node::Empty {
                result += 1;
            }
        }
        18 => {
            if board[10] == game::Node::Empty {
                result += 1;
            }
            if board[19] == game::Node::Empty {
                result += 1;
            }
        }
        19 => {
            if board[16] == game::Node::Empty {
                result += 1;
            }
            if board[18] == game::Node::Empty {
                result += 1;
            }
            if board[20] == game::Node::Empty {
                result += 1;
            }
            if board[22] == game::Node::Empty {
                result += 1;
            }
        }
        20 => {
            if board[13] == game::Node::Empty {
                result += 1;
            }
            if board[19] == game::Node::Empty {
                result += 1;
            }
        }
        21 => {
            if board[9] == game::Node::Empty {
                result += 1;
            }
            if board[22] == game::Node::Empty {
                result += 1;
            }
        }
        22 => {
            if board[19] == game::Node::Empty {
                result += 1;
            }
            if board[21] == game::Node::Empty {
                result += 1;
            }
            if board[23] == game::Node::Empty {
                result += 1;
            }
        }
        23 => {
            if board[14] == game::Node::Empty {
                result += 1;
            }
            if board[22] == game::Node::Empty {
                result += 1;
            }
        }
        _ => ()
    }

    result
}
