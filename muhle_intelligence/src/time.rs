use crate::game;

// https://web.archive.org/web/20071028092710/http://www.brucemo.com/compchess/programming/time.htm

pub fn sudden_death_time_control(wtime: Option<u32>, btime: Option<u32>, x: u32, position: &game::Position) -> u32 {
    let time = match position.player {
        game::Player::White => wtime.unwrap_or(u32::MAX),
        game::Player::Black => btime.unwrap_or(u32::MAX),
    };

    time / x
}
