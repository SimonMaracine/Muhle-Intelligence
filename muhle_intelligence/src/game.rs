use std::mem;
use std::str::FromStr;

use regex;

pub type Idx = i32;
pub const NULL_INDEX: Idx = -1;

#[derive(Debug, Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Player {
    #[default]
    White = 1,
    Black = 2,
}

#[derive(Debug, Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Node {
    #[default]
    Empty = 0,
    White = 1,
    Black = 2,
}

fn index_from_string(string: &str) -> Result<Idx, String> {
    match string {
        "a7" => Ok(0),
        "d7" => Ok(1),
        "g7" => Ok(2),
        "b6" => Ok(3),
        "d6" => Ok(4),
        "f6" => Ok(5),
        "c5" => Ok(6),
        "d5" => Ok(7),
        "e5" => Ok(8),
        "a4" => Ok(9),
        "b4" => Ok(10),
        "c4" => Ok(11),
        "e4" => Ok(12),
        "f4" => Ok(13),
        "g4" => Ok(14),
        "c3" => Ok(15),
        "d3" => Ok(16),
        "e3" => Ok(17),
        "b2" => Ok(18),
        "d2" => Ok(19),
        "f2" => Ok(20),
        "a1" => Ok(21),
        "d1" => Ok(22),
        "g1" => Ok(23),
        _ => Err(String::from("Invalid index string")),
    }
}

fn string_from_index(index: Idx) -> Result<&'static str, String> {
    match index {
        0 => Ok("a7"),
        1 => Ok("d7"),
        2 => Ok("g7"),
        3 => Ok("b6"),
        4 => Ok("d6"),
        5 => Ok("f6"),
        6 => Ok("c5"),
        7 => Ok("d5"),
        8 => Ok("e5"),
        9 => Ok("a4"),
        10 => Ok("b4"),
        11 => Ok("c4"),
        12 => Ok("e4"),
        13 => Ok("f4"),
        14 => Ok("g4"),
        15 => Ok("c3"),
        16 => Ok("d3"),
        17 => Ok("e3"),
        18 => Ok("b2"),
        19 => Ok("d2"),
        20 => Ok("f2"),
        21 => Ok("a1"),
        22 => Ok("d1"),
        23 => Ok("g1"),
        _ => Err(String::from("Invalid index number")),
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Move {
    Place {
        place_index: Idx,
    },
    PlaceCapture {
        place_index: Idx,
        capture_index: Idx,
    },
    Move {
        source_index: Idx,
        destination_index: Idx,
    },
    MoveCapture {
        source_index: Idx,
        destination_index: Idx,
        capture_index: Idx,
    },
}

impl Move {
    pub fn new_place(place_index: Idx) -> Self {
        Self::Place { place_index }
    }

    pub fn new_place_capture(place_index: Idx, capture_index: Idx) -> Self {
        Self::PlaceCapture { place_index, capture_index }
    }

    pub fn new_move(source_index: Idx, destination_index: Idx) -> Self {
        Self::Move { source_index, destination_index }
    }

    pub fn new_move_capture(source_index: Idx, destination_index: Idx, capture_index: Idx) -> Self {
        Self::MoveCapture { source_index, destination_index, capture_index }
    }
}

impl Default for Move {
    fn default() -> Self {
        Move::Place { place_index: NULL_INDEX }
    }
}

impl FromStr for Move {
    type Err = String;

    fn from_str(string: &str) -> Result<Self, Self::Err> {
        let tokens = string.split("-x").collect::<Vec<_>>();

        match tokens.len() {
            1 => {
                let place_index = index_from_string(tokens[0])?;

                Ok(Self::new_place(place_index))
            }
            2 => {
                if let None = string.find("-") {
                    let place_index = index_from_string(tokens[0])?;
                    let capture_index = index_from_string(tokens[1])?;

                    Ok(Self::new_place_capture(place_index, capture_index))
                } else {
                    let source_index = index_from_string(tokens[0])?;
                    let destination_index = index_from_string(tokens[1])?;

                    Ok(Self::new_move(source_index, destination_index))
                }
            }
            3 => {
                let source_index = index_from_string(tokens[0])?;
                let destination_index = index_from_string(tokens[1])?;
                let capture_index = index_from_string(tokens[2])?;

                Ok(Self::new_move_capture(source_index, destination_index, capture_index))
            }
            _ => Err(String::from("Invalid move string"))
        }
    }
}

impl ToString for Move {
    fn to_string(&self) -> String {
        const ERR_MSG: &str = "An invalid move shouldn't be used";

        let mut result = String::new();

        match *self {
            Self::Place { place_index } => {
                result.push_str(string_from_index(place_index).expect(ERR_MSG));
            }
            Self::PlaceCapture { place_index, capture_index } => {
                result.push_str(string_from_index(place_index).expect(ERR_MSG));
                result.push('x');
                result.push_str(string_from_index(capture_index).expect(ERR_MSG));
            }
            Self::Move { source_index, destination_index } => {
                result.push_str(string_from_index(source_index).expect(ERR_MSG));
                result.push('-');
                result.push_str(string_from_index(destination_index).expect(ERR_MSG));
            }
            Self::MoveCapture { source_index, destination_index, capture_index } => {
                result.push_str(string_from_index(source_index).expect(ERR_MSG));
                result.push('-');
                result.push_str(string_from_index(destination_index).expect(ERR_MSG));
                result.push('x');
                result.push_str(string_from_index(capture_index).expect(ERR_MSG));
            }
        }

        result
    }
}

pub type Board = [Node; 24];

#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct Position {
    pub board: Board,
    pub player: Player,
    pub plies: i32,
}

impl Position {
    fn parse_pieces(string: &str) -> (Vec<Idx>, Player) {
        let player = match &string[0..1] {
            "w" => Player::White,
            "b" => Player::Black,
            _ => panic!("Invalid position string"),
        };

        let tokens = string[1..].split(",");
        let mut pieces = Vec::new();

        for token in tokens {
            if token.is_empty() {
                continue;
            }

            pieces.push(index_from_string(token).expect("Position string should be valid as per the regex"));
        }

        (pieces, player)
    }
}

impl FromStr for Position {
    type Err = String;

    fn from_str(string: &str) -> Result<Self, Self::Err> {
        if !string.is_ascii() {
            return Err(String::from("Invalid position string"));
        }

        let re = regex::Regex::new(r"^(w|b):(w|b)([a-g][1-7])?(,[a-g][1-7])*:(w|b)([a-g][1-7])?(,[a-g][1-7])*:[0-9]{1,3}$");

        if !re.expect("The expression should be constant").is_match(string) {
            return Err(String::from("Invalid position string"));
        }

        let tokens = string.split(":").collect::<Vec<_>>();

        assert_eq!(tokens.len(), 4);

        let player = match tokens[0] {
            "w" => Player::White,
            "b" => Player::Black,
            _ => panic!("Invalid position string"),
        };

        let pieces1 = Self::parse_pieces(tokens[1]);
        let pieces2 = Self::parse_pieces(tokens[2]);
        let turns = tokens[3].parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?;

        if pieces1.1 == pieces2.1 {
            return Err(String::from("Invalid position string"));
        }

        if turns < 1 {
            return Err(String::from("Invalid position string"));
        }

        let mut position = Position::default();

        position.player = player;

        for index in pieces1.0 {
            assert!(index >= 0 && index < 24);

            position.board[index as usize] = player_piece(pieces1.1);
        }

        for index in pieces2.0 {
            assert!(index >= 0 && index < 24);

            position.board[index as usize] = player_piece(pieces2.1);
        }

        position.plies = (turns - 1) * 2 + (player == Player::Black) as i32;

        Ok(position)
    }
}

#[derive(Debug, Default, Clone, PartialEq, Eq)]
pub struct GamePosition {
    pub position: Position,
    pub plies_no_advancement: i32,
}

impl GamePosition {
    pub fn play_move(&mut self, move_: &Move) {
        match *move_ {
            Move::Place { place_index } => {
                assert!(self.position.board[place_index as usize] == Node::Empty);  // TODO use debug asserts

                self.position.board[place_index as usize] = player_piece(self.position.player);
            }
            Move::PlaceCapture { place_index, capture_index } => {
                assert!(self.position.board[place_index as usize] == Node::Empty);
                assert!(self.position.board[capture_index as usize] != Node::Empty);

                self.position.board[place_index as usize] = player_piece(self.position.player);
                self.position.board[capture_index as usize] = Node::Empty;

                self.plies_no_advancement = 0;
            }
            Move::Move { source_index, destination_index } => {
                assert!(self.position.board[source_index as usize] != Node::Empty);
                assert!(self.position.board[destination_index as usize] == Node::Empty);

                self.position.board.swap(source_index as usize, destination_index as usize);

                self.plies_no_advancement += 1;
            }
            Move::MoveCapture { source_index, destination_index, capture_index } => {
                assert!(self.position.board[source_index as usize] != Node::Empty);
                assert!(self.position.board[destination_index as usize] == Node::Empty);
                assert!(self.position.board[capture_index as usize] != Node::Empty);

                self.position.board.swap(source_index as usize, destination_index as usize);
                self.position.board[capture_index as usize] = Node::Empty;

                self.plies_no_advancement = 0;
            }
        }

        self.position.player = opponent(self.position.player);
        self.position.plies += 1;
    }
}

pub struct SearchNode<'a> {
    pub position: GamePosition,
    pub previous: Option<&'a SearchNode<'a>>,
}

impl<'a> SearchNode<'a> {
    pub fn from_position(position: &GamePosition) -> Self {
        Self {
            position: position.clone(),
            previous: None,
        }
    }

    pub fn from_node(node: &'a SearchNode) -> Self {
        Self {
            position: node.position.clone(),
            previous: Some(node),
        }
    }

    pub fn play_move(&mut self, move_: &Move) {
        match *move_ {
            Move::Place { place_index } => {
                assert!(self.position.position.board[place_index as usize] == Node::Empty);  // TODO use debug asserts

                self.position.position.board[place_index as usize] = player_piece(self.position.position.player);
            }
            Move::PlaceCapture { place_index, capture_index } => {
                assert!(self.position.position.board[place_index as usize] == Node::Empty);
                assert!(self.position.position.board[capture_index as usize] != Node::Empty);

                self.position.position.board[place_index as usize] = player_piece(self.position.position.player);
                self.position.position.board[capture_index as usize] = Node::Empty;

                self.position.plies_no_advancement = 0;
                self.previous = None;
            }
            Move::Move { source_index, destination_index } => {
                assert!(self.position.position.board[source_index as usize] != Node::Empty);
                assert!(self.position.position.board[destination_index as usize] == Node::Empty);

                self.position.position.board.swap(source_index as usize, destination_index as usize);

                self.position.plies_no_advancement += 1;
            }
            Move::MoveCapture { source_index, destination_index, capture_index } => {
                assert!(self.position.position.board[source_index as usize] != Node::Empty);
                assert!(self.position.position.board[destination_index as usize] == Node::Empty);
                assert!(self.position.position.board[capture_index as usize] != Node::Empty);

                self.position.position.board.swap(source_index as usize, destination_index as usize);
                self.position.position.board[capture_index as usize] = Node::Empty;

                self.position.plies_no_advancement = 0;
                self.previous = None;
            }
        }

        self.position.position.player = opponent(self.position.position.player);
        self.position.position.plies += 1;
    }
}

pub fn is_game_over_material(node: &SearchNode) -> bool {
    if node.position.position.plies < 18 {
        return false;
    }

    return count_player_pieces(&node.position.position) < 3;
}

pub fn count_player_pieces(position: &Position) -> i32 {
    let target = player_piece(position.player);
    let mut result = 0;

    for node in position.board {
        result += (node == target) as i32;
    }

    result
}

pub fn player_piece(player: Player) -> Node {
    let integer = player as u32;
    unsafe { mem::transmute(integer) }
}

pub fn opponent(player: Player) -> Player {
    match player {
        Player::White => Player::Black,
        Player::Black => Player::White,
    }
}
