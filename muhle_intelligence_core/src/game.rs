use std::str::FromStr;
use std::num::ParseIntError;

use regex;

use crate::various;

pub type Idx = i32;
pub static INVALID_INDEX: Idx = -1;

#[derive(Debug, Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Player {
    #[default]
    White = 1,
    Black = 2,
}

#[derive(Debug, Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Piece {
    #[default]
    None,
    White,
    Black,
}

#[derive(Debug, PartialEq, Eq)]
pub enum Move {
    Place {
        place_index: Idx,
    },
    PlaceTake {
        place_index: Idx,
        take_index: Idx,
    },
    Move {
        source_index: Idx,
        destination_index: Idx,
    },
    MoveTake {
        source_index: Idx,
        destination_index: Idx,
        take_index: Idx,
    },
}

impl Move {
    pub fn new_place(place_index: Idx) -> Self {
        Self::Place { place_index }
    }

    pub fn new_place_take(place_index: Idx, take_index: Idx) -> Self {
        Self::PlaceTake { place_index, take_index }
    }

    pub fn new_move(source_index: Idx, destination_index: Idx) -> Self {
        Self::Move { source_index, destination_index }
    }

    pub fn new_move_take(source_index: Idx, destination_index: Idx, take_index: Idx) -> Self {
        Self::MoveTake { source_index, destination_index, take_index }
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
}

impl Default for Move {
    fn default() -> Self {
        Move::Place { place_index: INVALID_INDEX }
    }
}

impl FromStr for Move {
    type Err = String;

    fn from_str(string: &str) -> Result<Self, Self::Err> {
        match &string[0..1] {
            "P" => {
                let place_index = Self::index_from_string(&string[1..3])?;

                match string.get(3..4) {
                    Some(_) => {
                        let take_index = Self::index_from_string(&string[4..6])?;

                        Ok(Self::new_place_take(place_index, take_index))
                    }
                    None => {
                        Ok(Self::new_place(place_index))
                    }
                }
            }
            "M" => {
                let source_index = Self::index_from_string(&string[1..3])?;
                let destination_index = Self::index_from_string(&string[4..6])?;

                match string.get(6..7) {
                    Some(_) => {
                        let take_index = Self::index_from_string(&string[7..9])?;

                        Ok(Self::new_move_take(source_index, destination_index, take_index))
                    }
                    None => {
                        Ok(Self::new_move(source_index, destination_index))
                    }
                }
            }
            _ => return Err(String::from("Invalid move string")),
        }
    }
}

impl ToString for Move {
    fn to_string(&self) -> String {
        let mut result = String::new();

        match *self {
            Self::Place { place_index } => {
                result.push('P');
                result.push_str(Self::string_from_index(place_index).unwrap());
            }
            Self::PlaceTake { place_index, take_index } => {
                result.push('P');
                result.push_str(Self::string_from_index(place_index).unwrap());
                result.push('T');
                result.push_str(Self::string_from_index(take_index).unwrap());
            }
            Self::Move { source_index, destination_index } => {
                result.push('M');
                result.push_str(Self::string_from_index(source_index).unwrap());
                result.push('-');
                result.push_str(Self::string_from_index(destination_index).unwrap());
            }
            Self::MoveTake { source_index, destination_index, take_index } => {
                result.push('M');
                result.push_str(Self::string_from_index(source_index).unwrap());
                result.push('-');
                result.push_str(Self::string_from_index(destination_index).unwrap());
                result.push('T');
                result.push_str(Self::string_from_index(take_index).unwrap());
            }
        }

        result
    }
}

pub type Board = [Piece; 24];

#[derive(Debug, Default, PartialEq, Eq)]
pub struct Position {
    pub board: Board,
    pub player: Player,
    pub plies: u32,
    pub plies_without_advancement: u32,
}

impl Position {
    pub fn play_move(&mut self, move_: &Move) {
        match *move_ {
            Move::Place { place_index } => {
                assert!(self.board[place_index as usize] == Piece::None);

                self.board[place_index as usize] = various::player_piece(self.player);

                self.plies_without_advancement += 1;
            }
            Move::PlaceTake { place_index, take_index } => {
                assert!(self.board[place_index as usize] == Piece::None);
                assert!(self.board[take_index as usize] != Piece::None);

                self.board[place_index as usize] = various::player_piece(self.player);
                self.board[take_index as usize] = Piece::None;

                self.plies_without_advancement = 0;
            }
            Move::Move { source_index, destination_index } => {
                assert!(self.board[source_index as usize] != Piece::None);
                assert!(self.board[destination_index as usize] == Piece::None);

                self.board.swap(source_index as usize, destination_index as usize);

                self.plies_without_advancement += 1;
            }
            Move::MoveTake { source_index, destination_index, take_index } => {
                assert!(self.board[source_index as usize] != Piece::None);
                assert!(self.board[destination_index as usize] == Piece::None);
                assert!(self.board[take_index as usize] != Piece::None);

                self.board.swap(source_index as usize, destination_index as usize);
                self.board[take_index as usize] = Piece::None;

                self.plies_without_advancement = 0;
            }
        }

        self.player = various::opponent(self.player);
        self.plies += 1;
    }

    fn parse_pieces(string: &str, i: &mut usize) -> (Vec<Idx>, Player) {
        let mut pieces = Vec::new();
        let player;

        match string.chars().nth(*i) {
            Some('w') => player = Player::White,
            Some('b') => player = Player::Black,
            _ => panic!("Invalid position string"),
        }

        *i += 2;

        loop {
            let piece = Self::parse_piece(string, i);

            if let Some(index) = piece {
                pieces.push(index);
            } else {
                break;
            }
        }

        (pieces, player)
    }

    fn parse_piece(string: &str, i: &mut usize) -> Option<Idx> {
        let index = Self::parse_integer::<Idx>(string, i);

        if let Some(',') = string.chars().nth(*i) {
            *i += 1;
        }

        index
    }

    fn parse_integer<T>(string: &str, i: &mut usize) -> Option<T>
        where T: FromStr<Err = ParseIntError> {
        let mut result = String::new();

        let mut characters = string.chars().skip(*i);

        loop {
            let character = characters.next();

            if let Some(character) = character {
                if character.is_numeric() {
                    *i += 1;
                    result.push(character);
                } else {
                    break;
                }
            } else {
                break;
            }
        }

        if result.is_empty() {
            return None;
        }

        Some(T::from_str(&result).unwrap())
    }
}

impl FromStr for Position {
    type Err = String;

    fn from_str(string: &str) -> Result<Self, Self::Err> {
        if !string.is_ascii() {
            return Err(String::from("Invalid position string"));
        }

        let re = regex::Regex::new(
            r"^(w|b):([0-9]{1,2})?(,[0-9]{1,2})*;(w|b):([0-9]{1,2})?(,[0-9]{1,2})*;(w|b);[0-9]{1,4};[0-9]{1,4}$"
        );

        if !re.unwrap().is_match(string) {
            return Err(String::from("Invalid position string"));
        }

        let mut position = Self::default();

        let mut i = 0;

        let (pieces1, player1) = Self::parse_pieces(string, &mut i);

        i += 1;

        let (pieces2, player2) = Self::parse_pieces(string, &mut i);

        i += 1;

        if player1 == player2 {
            panic!("Invalid position string");
        }

        for index in pieces1 {
            if !(index >= 0 && index < 24) {
                return Err(String::from("Invalid position string"));
            }

            position.board[index as usize] = various::player_piece(player1);
        }

        for index in pieces2 {
            if !(index >= 0 && index < 24) {
                return Err(String::from("Invalid position string"));
            }

            position.board[index as usize] = various::player_piece(player2);
        }

        match string.chars().nth(i) {
            Some('w') => position.player = Player::White,
            Some('b') => position.player = Player::Black,
            _ => panic!("Invalid position string"),
        }

        i += 2;

        let plies = Self::parse_integer::<u32>(string, &mut i);

        i += 1;

        let plies_without_advancement = Self::parse_integer::<u32>(string, &mut i);

        position.plies = plies.unwrap();
        position.plies_without_advancement = plies_without_advancement.unwrap();

        Ok(position)
    }
}
