use crate::game;

pub fn generate_moves(node: &game::SearchNode) -> Vec<game::Move> {
    if node.position.position.plies < 18 {
        generate_moves_phase1(node.position.position.board.clone(), node.position.position.player)
    } else {
        assert!(game::count_player_pieces(&node.position.position) >= 3);

        if game::count_player_pieces(&node.position.position) == 3 {
            generate_moves_phase3(node.position.position.board.clone(), node.position.position.player)
        } else {
            generate_moves_phase2(node.position.position.board.clone(), node.position.position.player)
        }
    }
}

fn generate_moves_phase1(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 as game::Idx {
        if board[i as usize] != game::Node::Empty {
            continue;
        }

        make_place_move(&mut board, player, i);

        if is_mill(&board, player, i) {
            let opponent_player = game::opponent(player);
            let all_in_mills = all_pieces_in_mills(&board, opponent_player);

            for j in 0..24 as game::Idx {
                if board[j as usize] != game::as_node(opponent_player) {
                    continue;
                }

                if is_mill(&board, opponent_player, j) && !all_in_mills {
                    continue;
                }

                moves.push(game::Move::new_place_capture(i, j));
            }
        } else {
            moves.push(game::Move::new_place(i));
        }

        unmake_place_move(&mut board, i);
    }

    moves
}

fn generate_moves_phase2(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 as game::Idx {
        if board[i as usize] != game::as_node(player) {
            continue;
        }

        let free_positions = neighbor_free_positions(&board, i);

        for j in 0..free_positions.len() as game::Idx {
            make_move_move(&mut board, i, free_positions[j as usize]);

            if is_mill(&board, player, free_positions[j as usize]) {
                let opponent_player = game::opponent(player);
                let all_in_mills = all_pieces_in_mills(&board, opponent_player);

                for k in 0..24 as game::Idx {
                    if board[k as usize] != game::as_node(opponent_player) {
                        continue;
                    }

                    if is_mill(&board, opponent_player, k) && !all_in_mills {
                        continue;
                    }

                    moves.push(game::Move::new_move_capture(i, free_positions[j as usize], k));
                }
            } else {
                moves.push(game::Move::new_move(i, free_positions[j as usize]));
            }

            unmake_move_move(&mut board, i, free_positions[j as usize]);
        }
    }

    moves
}

fn generate_moves_phase3(mut board: game::Board, player: game::Player) -> Vec<game::Move> {
    let mut moves = Vec::new();

    for i in 0..24 as game::Idx {
        if board[i as usize] != game::as_node(player) {
            continue;
        }

        for j in 0..24 as game::Idx {
            if board[j as usize] != game::Node::Empty {
                continue;
            }

            make_move_move(&mut board, i, j);

            if is_mill(&board, player, j) {
                let opponent_player = game::opponent(player);
                let all_in_mills = all_pieces_in_mills(&board, opponent_player);

                for k in 0..24 as game::Idx {
                    if board[k as usize] != game::as_node(opponent_player) {
                        continue;
                    }

                    if is_mill(&board, opponent_player, k) && !all_in_mills {
                        continue;
                    }

                    moves.push(game::Move::new_move_capture(i, j, k));
                }
            } else {
                moves.push(game::Move::new_move(i, j));
            }

            unmake_move_move(&mut board, i, j);
        }
    }

    moves
}

fn make_place_move(board: &mut game::Board, player: game::Player, place_index: game::Idx) {
    assert!(board[place_index as usize] == game::Node::Empty);

    board[place_index as usize] = game::as_node(player);
}

fn unmake_place_move(board: &mut game::Board, place_index: game::Idx) {
    assert!(board[place_index as usize] != game::Node::Empty);

    board[place_index as usize] = game::Node::Empty;
}

fn make_move_move(board: &mut game::Board, source_index: game::Idx, destination_index: game::Idx) {
    assert!(board[source_index as usize] != game::Node::Empty);
    assert!(board[destination_index as usize] == game::Node::Empty);

    board.swap(source_index as usize, destination_index as usize);
}

fn unmake_move_move(board: &mut game::Board, source_index: game::Idx, destination_index: game::Idx) {
    assert!(board[source_index as usize] == game::Node::Empty);
    assert!(board[destination_index as usize] != game::Node::Empty);

    board.swap(source_index as usize, destination_index as usize);
}

fn is_mill(board: &game::Board, player: game::Player, index: game::Idx) -> bool {
    let node = game::as_node(player);

    assert!(board[index as usize] == node);

    match index {
        0 => return board[1] == node && board[2] == node || board[9] == node && board[21] == node,
        1 => return board[0] == node && board[2] == node || board[4] == node && board[7] == node,
        2 => return board[0] == node && board[1] == node || board[14] == node && board[23] == node,
        3 => return board[4] == node && board[5] == node || board[10] == node && board[18] == node,
        4 => return board[3] == node && board[5] == node || board[1] == node && board[7] == node,
        5 => return board[3] == node && board[4] == node || board[13] == node && board[20] == node,
        6 => return board[7] == node && board[8] == node || board[11] == node && board[15] == node,
        7 => return board[6] == node && board[8] == node || board[1] == node && board[4] == node,
        8 => return board[6] == node && board[7] == node || board[12] == node && board[17] == node,
        9 => return board[0] == node && board[21] == node || board[10] == node && board[11] == node,
        10 => return board[9] == node && board[11] == node || board[3] == node && board[18] == node,
        11 => return board[9] == node && board[10] == node || board[6] == node && board[15] == node,
        12 => return board[13] == node && board[14] == node || board[8] == node && board[17] == node,
        13 => return board[12] == node && board[14] == node || board[5] == node && board[20] == node,
        14 => return board[12] == node && board[13] == node || board[2] == node && board[23] == node,
        15 => return board[16] == node && board[17] == node || board[6] == node && board[11] == node,
        16 => return board[15] == node && board[17] == node || board[19] == node && board[22] == node,
        17 => return board[15] == node && board[16] == node || board[8] == node && board[12] == node,
        18 => return board[19] == node && board[20] == node || board[3] == node && board[10] == node,
        19 => return board[18] == node && board[20] == node || board[16] == node && board[22] == node,
        20 => return board[18] == node && board[19] == node || board[5] == node && board[13] == node,
        21 => return board[22] == node && board[23] == node || board[0] == node && board[9] == node,
        22 => return board[21] == node && board[23] == node || board[16] == node && board[19] == node,
        23 => return board[21] == node && board[22] == node || board[2] == node && board[14] == node,
        _ => ()
    }

    unreachable!();
}

fn all_pieces_in_mills(board: &game::Board, player: game::Player) -> bool {
    for i in 0..24 as game::Idx {
        if board[i as usize] != game::as_node(player) {
            continue;
        }

        if !is_mill(board, player, i) {
            return false;
        }
    }

    true
}

fn neighbor_free_positions(board: &game::Board, index: game::Idx) -> Vec<game::Idx> {
    let mut result = Vec::new();
    result.reserve(4);

    match index {
        0 => {
            if board[1] == game::Node::Empty {
                result.push(1);
            }
            if board[9] == game::Node::Empty {
                result.push(9);
            }
        }
        1 => {
            if board[0] == game::Node::Empty {
                result.push(0);
            }
            if board[2] == game::Node::Empty {
                result.push(2);
            }
            if board[4] == game::Node::Empty {
                result.push(4);
            }
        }
        2 => {
            if board[1] == game::Node::Empty {
                result.push(1);
            }
            if board[14] == game::Node::Empty {
                result.push(14);
            }
        }
        3 => {
            if board[4] == game::Node::Empty {
                result.push(4);
            }
            if board[10] == game::Node::Empty {
                result.push(10);
            }
        }
        4 => {
            if board[1] == game::Node::Empty {
                result.push(1);
            }
            if board[3] == game::Node::Empty {
                result.push(3);
            }
            if board[5] == game::Node::Empty {
                result.push(5);
            }
            if board[7] == game::Node::Empty {
                result.push(7);
            }
        }
        5 => {
            if board[4] == game::Node::Empty {
                result.push(4);
            }
            if board[13] == game::Node::Empty {
                result.push(13);
            }
        }
        6 => {
            if board[7] == game::Node::Empty {
                result.push(7);
            }
            if board[11] == game::Node::Empty {
                result.push(11);
            }
        }
        7 => {
            if board[4] == game::Node::Empty {
                result.push(4);
            }
            if board[6] == game::Node::Empty {
                result.push(6);
            }
            if board[8] == game::Node::Empty {
                result.push(8);
            }
        }
        8 => {
            if board[7] == game::Node::Empty {
                result.push(7);
            }
            if board[12] == game::Node::Empty {
                result.push(12);
            }
        }
        9 => {
            if board[0] == game::Node::Empty {
                result.push(0);
            }
            if board[10] == game::Node::Empty {
                result.push(10);
            }
            if board[21] == game::Node::Empty {
                result.push(21);
            }
        }
        10 => {
            if board[3] == game::Node::Empty {
                result.push(3);
            }
            if board[9] == game::Node::Empty {
                result.push(9);
            }
            if board[1] == game::Node::Empty {
                result.push(1);
            }
            if board[18] == game::Node::Empty {
                result.push(18);
            }
        }
        11 => {
            if board[6] == game::Node::Empty {
                result.push(6);
            }
            if board[10] == game::Node::Empty {
                result.push(10);
            }
            if board[15] == game::Node::Empty {
                result.push(15);
            }
        }
        12 => {
            if board[8] == game::Node::Empty {
                result.push(8);
            }
            if board[13] == game::Node::Empty {
                result.push(13);
            }
            if board[17] == game::Node::Empty {
                result.push(17);
            }
        }
        13 => {
            if board[5] == game::Node::Empty {
                result.push(5);
            }
            if board[12] == game::Node::Empty {
                result.push(12);
            }
            if board[14] == game::Node::Empty {
                result.push(14);
            }
            if board[20] == game::Node::Empty {
                result.push(20);
            }
        }
        14 => {
            if board[2] == game::Node::Empty {
                result.push(2);
            }
            if board[13] == game::Node::Empty {
                result.push(13);
            }
            if board[23] == game::Node::Empty {
                result.push(23);
            }
        }
        15 => {
            if board[11] == game::Node::Empty {
                result.push(11);
            }
            if board[16] == game::Node::Empty {
                result.push(16);
            }
        }
        16 => {
            if board[15] == game::Node::Empty {
                result.push(15);
            }
            if board[17] == game::Node::Empty {
                result.push(17);
            }
            if board[19] == game::Node::Empty {
                result.push(19);
            }
        }
        17 => {
            if board[12] == game::Node::Empty {
                result.push(12);
            }
            if board[16] == game::Node::Empty {
                result.push(16);
            }
        }
        18 => {
            if board[10] == game::Node::Empty {
                result.push(10);
            }
            if board[19] == game::Node::Empty {
                result.push(19);
            }
        }
        19 => {
            if board[16] == game::Node::Empty {
                result.push(16);
            }
            if board[18] == game::Node::Empty {
                result.push(18);
            }
            if board[20] == game::Node::Empty {
                result.push(20);
            }
            if board[22] == game::Node::Empty {
                result.push(22);
            }
        }
        20 => {
            if board[13] == game::Node::Empty {
                result.push(13);
            }
            if board[19] == game::Node::Empty {
                result.push(19);
            }
        }
        21 => {
            if board[9] == game::Node::Empty {
                result.push(9);
            }
            if board[22] == game::Node::Empty {
                result.push(22);
            }
        }
        22 => {
            if board[19] == game::Node::Empty {
                result.push(19);
            }
            if board[21] == game::Node::Empty {
                result.push(21);
            }
            if board[23] == game::Node::Empty {
                result.push(23);
            }
        }
        23 => {
            if board[14] == game::Node::Empty {
                result.push(14);
            }
            if board[22] == game::Node::Empty {
                result.push(22);
            }
        }
        _ => ()
    }

    result
}
