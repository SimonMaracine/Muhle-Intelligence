use std::hint::unreachable_unchecked;

use crate::game;

pub type Eval = i32;

pub const WINDOW_MAX: Eval = 500;
pub const WINDOW_MIN: Eval = -WINDOW_MAX;

pub const MAX: Eval = WINDOW_MAX - 1;
pub const MIN: Eval = WINDOW_MIN + 1;

const PLAYER_EVAL: Eval = 10;
const FREEDOM_EVAL: Eval = 1;
const ENDGAME_PLAYER_FREEDOM: i32 = 3 * 4;

pub fn static_evaluation<const P: i32>(node: &game::SearchNode) -> Eval {
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
            game::Node::White => white_freedom += piece_freedom::<P>(&node.position.position.board, i),
            game::Node::Black => black_freedom += piece_freedom::<P>(&node.position.position.board, i),
            game::Node::Empty => ()
        }
    }

    if node.position.position.plies >= P {
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

fn freedom(board: &game::Board, result: &mut i32, index: usize) {
    if board[index] == game::Node::Empty {
        *result += 1;
    }
}

fn piece_freedom<const P: i32>(board: &game::Board, index: usize) -> i32 {
    match P {
        game::NINE => piece_freedom9(board, index),
        game::TWELVE => piece_freedom12(board, index),
        _ => unsafe { unreachable_unchecked() },
    }
}

fn piece_freedom9(board: &game::Board, index: usize) -> i32 {
    let mut result = 0;

    match index {
        0 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 9);
        }
        1 => {
            freedom(board, &mut result, 0);
            freedom(board, &mut result, 2);
            freedom(board, &mut result, 4);
        }
        2 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 14);
        }
        3 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 10);
        }
        4 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 3);
            freedom(board, &mut result, 5);
            freedom(board, &mut result, 7);
        }
        5 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 13);
        }
        6 => {
            freedom(board, &mut result, 7);
            freedom(board, &mut result, 11);
        }
        7 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 6);
            freedom(board, &mut result, 8);
        }
        8 => {
            freedom(board, &mut result, 7);
            freedom(board, &mut result, 12);
        }
        9 => {
            freedom(board, &mut result, 0);
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 21);
        }
        10 => {
            freedom(board, &mut result, 3);
            freedom(board, &mut result, 9);
            freedom(board, &mut result, 11);
            freedom(board, &mut result, 18);
        }
        11 => {
            freedom(board, &mut result, 6);
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 15);
        }
        12 => {
            freedom(board, &mut result, 8);
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 17);
        }
        13 => {
            freedom(board, &mut result, 5);
            freedom(board, &mut result, 12);
            freedom(board, &mut result, 14);
            freedom(board, &mut result, 20);
        }
        14 => {
            freedom(board, &mut result, 2);
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 23);
        }
        15 => {
            freedom(board, &mut result, 11);
            freedom(board, &mut result, 16);
        }
        16 => {
            freedom(board, &mut result, 15);
            freedom(board, &mut result, 17);
            freedom(board, &mut result, 19);
        }
        17 => {
            freedom(board, &mut result, 12);
            freedom(board, &mut result, 16);
        }
        18 => {
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 19);
        }
        19 => {
            freedom(board, &mut result, 16);
            freedom(board, &mut result, 18);
            freedom(board, &mut result, 20);
            freedom(board, &mut result, 22);
        }
        20 => {
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 19);
        }
        21 => {
            freedom(board, &mut result, 9);
            freedom(board, &mut result, 22);
        }
        22 => {
            freedom(board, &mut result, 19);
            freedom(board, &mut result, 21);
            freedom(board, &mut result, 23);
        }
        23 => {
            freedom(board, &mut result, 14);
            freedom(board, &mut result, 22);
        }
        _ => ()
    }

    result
}

fn piece_freedom12(board: &game::Board, index: usize) -> i32 {
    let mut result = 0;

    match index {
        0 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 9);
            freedom(board, &mut result, 3);
        }
        1 => {
            freedom(board, &mut result, 0);
            freedom(board, &mut result, 2);
            freedom(board, &mut result, 4);
        }
        2 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 14);
            freedom(board, &mut result, 5);
        }
        3 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 0);
            freedom(board, &mut result, 6);
        }
        4 => {
            freedom(board, &mut result, 1);
            freedom(board, &mut result, 3);
            freedom(board, &mut result, 5);
            freedom(board, &mut result, 7);
        }
        5 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 2);
            freedom(board, &mut result, 8);
        }
        6 => {
            freedom(board, &mut result, 7);
            freedom(board, &mut result, 11);
            freedom(board, &mut result, 3);
        }
        7 => {
            freedom(board, &mut result, 4);
            freedom(board, &mut result, 6);
            freedom(board, &mut result, 8);
        }
        8 => {
            freedom(board, &mut result, 7);
            freedom(board, &mut result, 12);
            freedom(board, &mut result, 5);
        }
        9 => {
            freedom(board, &mut result, 0);
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 21);
        }
        10 => {
            freedom(board, &mut result, 3);
            freedom(board, &mut result, 9);
            freedom(board, &mut result, 11);
            freedom(board, &mut result, 18);
        }
        11 => {
            freedom(board, &mut result, 6);
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 15);
        }
        12 => {
            freedom(board, &mut result, 8);
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 17);
        }
        13 => {
            freedom(board, &mut result, 5);
            freedom(board, &mut result, 12);
            freedom(board, &mut result, 14);
            freedom(board, &mut result, 20);
        }
        14 => {
            freedom(board, &mut result, 2);
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 23);
        }
        15 => {
            freedom(board, &mut result, 11);
            freedom(board, &mut result, 16);
            freedom(board, &mut result, 18);
        }
        16 => {
            freedom(board, &mut result, 15);
            freedom(board, &mut result, 17);
            freedom(board, &mut result, 19);
        }
        17 => {
            freedom(board, &mut result, 12);
            freedom(board, &mut result, 16);
            freedom(board, &mut result, 20);
        }
        18 => {
            freedom(board, &mut result, 10);
            freedom(board, &mut result, 19);
            freedom(board, &mut result, 15);
            freedom(board, &mut result, 21);
        }
        19 => {
            freedom(board, &mut result, 16);
            freedom(board, &mut result, 18);
            freedom(board, &mut result, 20);
            freedom(board, &mut result, 22);
        }
        20 => {
            freedom(board, &mut result, 13);
            freedom(board, &mut result, 19);
            freedom(board, &mut result, 17);
            freedom(board, &mut result, 23);
        }
        21 => {
            freedom(board, &mut result, 9);
            freedom(board, &mut result, 22);
            freedom(board, &mut result, 18);
        }
        22 => {
            freedom(board, &mut result, 19);
            freedom(board, &mut result, 21);
            freedom(board, &mut result, 23);
        }
        23 => {
            freedom(board, &mut result, 14);
            freedom(board, &mut result, 22);
            freedom(board, &mut result, 20);
        }
        _ => ()
    }

    result
}
