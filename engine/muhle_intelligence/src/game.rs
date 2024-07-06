use crate::various;

pub type Idx = i32;
pub static INVALID_INDEX: Idx = -1;

#[derive(Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Player {
    #[default]
    White = 1,
    Black = 2,
}

#[derive(Default, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum Piece {
    #[default]
    None,
    White,
    Black,
}

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
}

impl Default for Move {
    fn default() -> Self {
        Move::Place { place_index: INVALID_INDEX }
    }
}

pub type Board = [Piece; 24];

#[derive(Default)]
pub struct Position {
    pub board: Board,
    pub player: Player,
    pub plies: u32,
    pub plies_without_advancement: u32,
}

impl Position {
    pub fn play_move(&mut self, r#move: &Move) {
        match r#move {
            Move::Place { place_index } => {
                self.board[*place_index as usize] = various::player_piece(self.player);

                self.plies_without_advancement += 1;
            }
            Move::PlaceTake { place_index, take_index } => {
                self.board[*place_index as usize] = various::player_piece(self.player);
                self.board[*take_index as usize] = Piece::None;

                self.plies_without_advancement = 0;
            }
            Move::Move { source_index, destination_index } => {
                self.board.swap(*source_index as usize, *destination_index as usize);

                self.plies_without_advancement += 1;
            }
            Move::MoveTake { source_index, destination_index, take_index } => {
                self.board.swap(*source_index as usize, *destination_index as usize);
                self.board[*take_index as usize] = Piece::None;

                self.plies_without_advancement = 0;
            }
        }

        self.player = various::opponent(self.player);
        self.plies += 1;
    }
}
