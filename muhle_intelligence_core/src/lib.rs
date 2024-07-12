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
    }
}
