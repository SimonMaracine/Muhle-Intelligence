use std::mem;

use crate::{game, search};

pub fn count_pieces(node: &search::SearchNode, piece: game::Piece) -> u32 {
    let mut result = 0;

    for p in node.board {
        result += (p == piece) as u32;
    }

    result
}

pub fn player_piece(player: game::Player) -> game::Piece {
    let integer = player as u32;
    unsafe { mem::transmute(integer) }
}

pub fn opponent(player: game::Player) -> game::Player {
    match player {
        game::Player::White => game::Player::Black,
        game::Player::Black => game::Player::White,
    }
}
