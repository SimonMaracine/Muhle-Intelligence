pub mod engine;
pub mod commands;
mod game;
mod search;
mod evaluation;
mod move_generation;
mod various;

#[cfg(test)]
mod test {
    use std::str::FromStr;

    use super::game;

    #[test]
    fn parse_position() {
        assert_eq!(
            game::Position::default(),
            game::Position::from_str("w:;b:;w;0;0").unwrap()
        );

        let position = game::Position {
            board: [
                game::Piece::Black,
                game::Piece::Black,
                game::Piece::White,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::None,
                game::Piece::White,
                game::Piece::Black,
                game::Piece::White,
            ],
            player: game::Player::Black,
            plies: 6,
            plies_without_advancement: 0,
        };

        assert_eq!(
            position,
            game::Position::from_str("w:2,21,23;b:0,1,22;b;6;0").unwrap()
        );

        assert!(game::Position::from_str("ofh3c;2bv73").is_err());
        assert!(game::Position::from_str("  w:;b:;w;0;0  ").is_err());
        assert!(game::Position::from_str("w:30;b:;w;0;0").is_err());
        assert!(game::Position::from_str("w:;b:30;w;0;0").is_err());
        assert!(game::Position::from_str("w:;b:;a;0;0").is_err());
        assert!(game::Position::from_str("w:;b:;w;99999;0").is_err());
        assert!(game::Position::from_str("w:;b:;w;0;99999").is_err());
        assert!(game::Position::from_str("b:;b:;w;0;0").is_err());
    }
}
