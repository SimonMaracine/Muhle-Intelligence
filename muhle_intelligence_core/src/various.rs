use std::mem;

use crate::{game, search, move_generation};

pub fn is_game_over_winner_material(node: &search::SearchNode) -> bool {
    if node.plies < 18 {
        return false;
    }

    let white_pieces = count_pieces(node, game::Piece::White);
    let black_pieces = count_pieces(node, game::Piece::Black);

    if white_pieces < 3 || black_pieces < 3 {
        return true;
    }

    false
}

pub fn is_game_over(position: &game::Position) -> bool {
    let node = search::SearchNode::from_position(position);

    if is_game_over_winner_material(&node) {
        return true;
    }

    if move_generation::generate_moves(&node).is_empty() {
        return true;
    }

    // FIXME repetition and 50-move rule

    false
}

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
