use std::hint::unreachable_unchecked;

use crate::game;

pub fn generate_moves<const P: i32>(node: &game::SearchNode) -> Vec<game::Move> {
    if node.position.position.plies < P {
        generate_moves_phase1::<P>(node.position.position.board.clone(), node.position.position.player)
    } else {
        debug_assert!(node.position.position.count_player_pieces() >= 3);

        if node.position.position.count_player_pieces() == 3 {
            generate_moves_phase3::<P>(node.position.position.board.clone(), node.position.position.player)
        } else {
            generate_moves_phase2::<P>(node.position.position.board.clone(), node.position.position.player)
        }
    }
}

fn generate_moves_phase1<const P: i32>(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 {
        if board[i] != game::Node::Empty {
            continue;
        }

        make_place_move(&mut board, player, i);

        if is_mill::<P>(&board, player, i) {
            let opponent_player = game::opponent(player);
            let all_in_mills = all_pieces_in_mills::<P>(&board, opponent_player);

            for j in 0..24 {
                if board[j] != game::as_node(opponent_player) {
                    continue;
                }

                if is_mill::<P>(&board, opponent_player, j) && !all_in_mills {
                    continue;
                }

                moves.push(game::Move::new_place_capture(i as game::Idx, j as game::Idx));
            }
        } else {
            moves.push(game::Move::new_place(i as game::Idx));
        }

        unmake_place_move(&mut board, i);
    }

    moves
}

fn generate_moves_phase2<const P: i32>(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 {
        if board[i] != game::as_node(player) {
            continue;
        }

        let free_positions = neighbor_free_positions::<P>(&board, i);

        for j in 0..free_positions.len() {
            make_move_move(&mut board, i, free_positions[j]);

            if is_mill::<P>(&board, player, free_positions[j]) {
                let opponent_player = game::opponent(player);
                let all_in_mills = all_pieces_in_mills::<P>(&board, opponent_player);

                for k in 0..24 {
                    if board[k] != game::as_node(opponent_player) {
                        continue;
                    }

                    if is_mill::<P>(&board, opponent_player, k) && !all_in_mills {
                        continue;
                    }

                    moves.push(game::Move::new_move_capture(i as game::Idx, free_positions[j] as game::Idx, k as game::Idx));
                }
            } else {
                moves.push(game::Move::new_move(i as game::Idx, free_positions[j] as game::Idx));
            }

            unmake_move_move(&mut board, i, free_positions[j]);
        }
    }

    moves
}

fn generate_moves_phase3<const P: i32>(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 {
        if board[i] != game::as_node(player) {
            continue;
        }

        for j in 0..24 {
            if board[j] != game::Node::Empty {
                continue;
            }

            make_move_move(&mut board, i, j);

            if is_mill::<P>(&board, player, j) {
                let opponent_player = game::opponent(player);
                let all_in_mills = all_pieces_in_mills::<P>(&board, opponent_player);

                for k in 0..24 {
                    if board[k] != game::as_node(opponent_player) {
                        continue;
                    }

                    if is_mill::<P>(&board, opponent_player, k) && !all_in_mills {
                        continue;
                    }

                    moves.push(game::Move::new_move_capture(i as game::Idx, j as game::Idx, k as game::Idx));
                }
            } else {
                moves.push(game::Move::new_move(i as game::Idx, j as game::Idx));
            }

            unmake_move_move(&mut board, i, j);
        }
    }

    moves
}

fn make_place_move(board: &mut game::Board, player: game::Player, place_index: usize) {
    debug_assert!(board[place_index] == game::Node::Empty);

    board[place_index] = game::as_node(player);
}

fn unmake_place_move(board: &mut game::Board, place_index: usize) {
    debug_assert!(board[place_index] != game::Node::Empty);

    board[place_index] = game::Node::Empty;
}

fn make_move_move(board: &mut game::Board, source_index: usize, destination_index: usize) {
    debug_assert!(board[source_index] != game::Node::Empty);
    debug_assert!(board[destination_index] == game::Node::Empty);

    board.swap(source_index, destination_index);
}

fn unmake_move_move(board: &mut game::Board, source_index: usize, destination_index: usize) {
    debug_assert!(board[source_index] == game::Node::Empty);
    debug_assert!(board[destination_index] != game::Node::Empty);

    board.swap(source_index, destination_index);
}

fn mill(board: &game::Board, node: game::Node, index1: usize, index2: usize) -> bool {
    board[index1] == node && board[index2] == node
}

fn is_mill<const P: i32>(board: &game::Board, player: game::Player, index: usize) -> bool {
    match P {
        game::NINE => is_mill9(board, player, index),
        game::TWELVE => is_mill12(board, player, index),
        _ => unsafe { unreachable_unchecked() },
    }
}

fn is_mill9(board: &game::Board, player: game::Player, index: usize) -> bool {
    let node = game::as_node(player);

    debug_assert!(board[index] == node);

    match index {
        0 => return mill(board, node, 1, 2) || mill(board, node, 9, 21),
        1 => return mill(board, node, 0, 2) || mill(board, node, 4, 7),
        2 => return mill(board, node, 0, 1) || mill(board, node, 14, 23),
        3 => return mill(board, node, 4, 5) || mill(board, node, 10, 18),
        4 => return mill(board, node, 3, 5) || mill(board, node, 1, 7),
        5 => return mill(board, node, 3, 4) || mill(board, node, 13, 20),
        6 => return mill(board, node, 7, 8) || mill(board, node, 11, 15),
        7 => return mill(board, node, 6, 8) || mill(board, node, 1, 4),
        8 => return mill(board, node, 6, 7) || mill(board, node, 12, 17),
        9 => return mill(board, node, 0, 21) || mill(board, node, 10, 11),
        10 => return mill(board, node, 9, 11) || mill(board, node, 3, 18),
        11 => return mill(board, node, 9, 10) || mill(board, node, 6, 15),
        12 => return mill(board, node, 13, 14) || mill(board, node, 8, 17),
        13 => return mill(board, node, 12, 14) || mill(board, node, 5, 20),
        14 => return mill(board, node, 12, 13) || mill(board, node, 2, 23),
        15 => return mill(board, node, 16, 17) || mill(board, node, 6, 11),
        16 => return mill(board, node, 15, 17) || mill(board, node, 19, 22),
        17 => return mill(board, node, 15, 16) || mill(board, node, 8, 12),
        18 => return mill(board, node, 19, 20) || mill(board, node, 3, 10),
        19 => return mill(board, node, 18, 20) || mill(board, node, 16, 22),
        20 => return mill(board, node, 18, 19) || mill(board, node, 5, 13),
        21 => return mill(board, node, 22, 23) || mill(board, node, 0, 9),
        22 => return mill(board, node, 21, 23) || mill(board, node, 16, 19),
        23 => return mill(board, node, 21, 22) || mill(board, node, 2, 14),
        _ => (),
    }

    unreachable!();
}

fn is_mill12(board: &game::Board, player: game::Player, index: usize) -> bool {
    let node = game::as_node(player);

    debug_assert!(board[index] == node);

    match index {
        0 => return mill(board, node, 1, 2) || mill(board, node, 9, 21) || mill(board, node, 3, 6),
        1 => return mill(board, node, 0, 2) || mill(board, node, 4, 7),
        2 => return mill(board, node, 0, 1) || mill(board, node, 14, 23) || mill(board, node, 5, 8),
        3 => return mill(board, node, 4, 5) || mill(board, node, 10, 18) || mill(board, node, 0, 6),
        4 => return mill(board, node, 3, 5) || mill(board, node, 1, 7),
        5 => return mill(board, node, 3, 4) || mill(board, node, 13, 20) || mill(board, node, 2, 8),
        6 => return mill(board, node, 7, 8) || mill(board, node, 11, 15) || mill(board, node, 0, 3),
        7 => return mill(board, node, 6, 8) || mill(board, node, 1, 4),
        8 => return mill(board, node, 6, 7) || mill(board, node, 12, 17) || mill(board, node, 2, 5),
        9 => return mill(board, node, 0, 21) || mill(board, node, 10, 11),
        10 => return mill(board, node, 9, 11) || mill(board, node, 3, 18),
        11 => return mill(board, node, 9, 10) || mill(board, node, 6, 15),
        12 => return mill(board, node, 13, 14) || mill(board, node, 8, 17),
        13 => return mill(board, node, 12, 14) || mill(board, node, 5, 20),
        14 => return mill(board, node, 12, 13) || mill(board, node, 2, 23),
        15 => return mill(board, node, 16, 17) || mill(board, node, 6, 11) || mill(board, node, 18, 21),
        16 => return mill(board, node, 15, 17) || mill(board, node, 19, 22),
        17 => return mill(board, node, 15, 16) || mill(board, node, 8, 12) || mill(board, node, 20, 23),
        18 => return mill(board, node, 19, 20) || mill(board, node, 3, 10) || mill(board, node, 15, 21),
        19 => return mill(board, node, 18, 20) || mill(board, node, 16, 22),
        20 => return mill(board, node, 18, 19) || mill(board, node, 5, 13) || mill(board, node, 17, 23),
        21 => return mill(board, node, 22, 23) || mill(board, node, 0, 9) || mill(board, node, 15, 18),
        22 => return mill(board, node, 21, 23) || mill(board, node, 16, 19),
        23 => return mill(board, node, 21, 22) || mill(board, node, 2, 14) || mill(board, node, 17, 20),
        _ => (),
    }

    unreachable!();
}

fn all_pieces_in_mills<const P: i32>(board: &game::Board, player: game::Player) -> bool {
    for i in 0..24 {
        if board[i] != game::as_node(player) {
            continue;
        }

        if !is_mill::<P>(board, player, i) {
            return false;
        }
    }

    true
}

fn neighbor(board: &game::Board, result: &mut Vec<usize>, index: usize) {
    if board[index] == game::Node::Empty {
        result.push(index);
    }
}

fn neighbor_free_positions<const P: i32>(board: &game::Board, index: usize) -> Vec<usize> {
    match P {
        game::NINE => neighbor_free_positions9(board, index),
        game::TWELVE => neighbor_free_positions12(board, index),
        _ => unsafe { unreachable_unchecked() },
    }
}

fn neighbor_free_positions9(board: &game::Board, index: usize) -> Vec<usize> {
    let mut result = Vec::new();
    result.reserve(4);

    match index {
        0 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 9);
        }
        1 => {
            neighbor(board, &mut result, 0);
            neighbor(board, &mut result, 2);
            neighbor(board, &mut result, 4);
        }
        2 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 14);
        }
        3 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 10);
        }
        4 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 3);
            neighbor(board, &mut result, 5);
            neighbor(board, &mut result, 7);
        }
        5 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 13);
        }
        6 => {
            neighbor(board, &mut result, 7);
            neighbor(board, &mut result, 11);
        }
        7 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 6);
            neighbor(board, &mut result, 8);
        }
        8 => {
            neighbor(board, &mut result, 7);
            neighbor(board, &mut result, 12);
        }
        9 => {
            neighbor(board, &mut result, 0);
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 21);
        }
        10 => {
            neighbor(board, &mut result, 3);
            neighbor(board, &mut result, 9);
            neighbor(board, &mut result, 11);
            neighbor(board, &mut result, 18);
        }
        11 => {
            neighbor(board, &mut result, 6);
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 15);
        }
        12 => {
            neighbor(board, &mut result, 8);
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 17);
        }
        13 => {
            neighbor(board, &mut result, 5);
            neighbor(board, &mut result, 12);
            neighbor(board, &mut result, 14);
            neighbor(board, &mut result, 20);
        }
        14 => {
            neighbor(board, &mut result, 2);
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 23);
        }
        15 => {
            neighbor(board, &mut result, 11);
            neighbor(board, &mut result, 16);
        }
        16 => {
            neighbor(board, &mut result, 15);
            neighbor(board, &mut result, 17);
            neighbor(board, &mut result, 19);
        }
        17 => {
            neighbor(board, &mut result, 12);
            neighbor(board, &mut result, 16);
        }
        18 => {
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 19);
        }
        19 => {
            neighbor(board, &mut result, 16);
            neighbor(board, &mut result, 18);
            neighbor(board, &mut result, 20);
            neighbor(board, &mut result, 22);
        }
        20 => {
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 19);
        }
        21 => {
            neighbor(board, &mut result, 9);
            neighbor(board, &mut result, 22);
        }
        22 => {
            neighbor(board, &mut result, 19);
            neighbor(board, &mut result, 21);
            neighbor(board, &mut result, 23);
        }
        23 => {
            neighbor(board, &mut result, 14);
            neighbor(board, &mut result, 22);
        }
        _ => ()
    }

    result
}

fn neighbor_free_positions12(board: &game::Board, index: usize) -> Vec<usize> {
    let mut result = Vec::new();
    result.reserve(4);

    match index {
        0 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 9);
            neighbor(board, &mut result, 3);
        }
        1 => {
            neighbor(board, &mut result, 0);
            neighbor(board, &mut result, 2);
            neighbor(board, &mut result, 4);
        }
        2 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 14);
            neighbor(board, &mut result, 5);
        }
        3 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 0);
            neighbor(board, &mut result, 6);
        }
        4 => {
            neighbor(board, &mut result, 1);
            neighbor(board, &mut result, 3);
            neighbor(board, &mut result, 5);
            neighbor(board, &mut result, 7);
        }
        5 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 2);
            neighbor(board, &mut result, 8);
        }
        6 => {
            neighbor(board, &mut result, 7);
            neighbor(board, &mut result, 11);
            neighbor(board, &mut result, 3);
        }
        7 => {
            neighbor(board, &mut result, 4);
            neighbor(board, &mut result, 6);
            neighbor(board, &mut result, 8);
        }
        8 => {
            neighbor(board, &mut result, 7);
            neighbor(board, &mut result, 12);
            neighbor(board, &mut result, 5);
        }
        9 => {
            neighbor(board, &mut result, 0);
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 21);
        }
        10 => {
            neighbor(board, &mut result, 3);
            neighbor(board, &mut result, 9);
            neighbor(board, &mut result, 11);
            neighbor(board, &mut result, 18);
        }
        11 => {
            neighbor(board, &mut result, 6);
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 15);
        }
        12 => {
            neighbor(board, &mut result, 8);
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 17);
        }
        13 => {
            neighbor(board, &mut result, 5);
            neighbor(board, &mut result, 12);
            neighbor(board, &mut result, 14);
            neighbor(board, &mut result, 20);
        }
        14 => {
            neighbor(board, &mut result, 2);
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 23);
        }
        15 => {
            neighbor(board, &mut result, 11);
            neighbor(board, &mut result, 16);
            neighbor(board, &mut result, 18);
        }
        16 => {
            neighbor(board, &mut result, 15);
            neighbor(board, &mut result, 17);
            neighbor(board, &mut result, 19);
        }
        17 => {
            neighbor(board, &mut result, 12);
            neighbor(board, &mut result, 16);
            neighbor(board, &mut result, 20);
        }
        18 => {
            neighbor(board, &mut result, 10);
            neighbor(board, &mut result, 19);
            neighbor(board, &mut result, 15);
            neighbor(board, &mut result, 21);
        }
        19 => {
            neighbor(board, &mut result, 16);
            neighbor(board, &mut result, 18);
            neighbor(board, &mut result, 20);
            neighbor(board, &mut result, 22);
        }
        20 => {
            neighbor(board, &mut result, 13);
            neighbor(board, &mut result, 19);
            neighbor(board, &mut result, 17);
            neighbor(board, &mut result, 23);
        }
        21 => {
            neighbor(board, &mut result, 9);
            neighbor(board, &mut result, 22);
            neighbor(board, &mut result, 18);
        }
        22 => {
            neighbor(board, &mut result, 19);
            neighbor(board, &mut result, 21);
            neighbor(board, &mut result, 23);
        }
        23 => {
            neighbor(board, &mut result, 14);
            neighbor(board, &mut result, 22);
            neighbor(board, &mut result, 20);
        }
        _ => ()
    }

    result
}
