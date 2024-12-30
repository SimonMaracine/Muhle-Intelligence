use crate::engine;

pub enum Option {
    Check {
        value: bool,
        default: bool,
    },
    Spin {
        value: i32,
        default: i32,
        min: i32,
        max: i32,
    },
    Combo {
        value: String,
        default: String,
        vars: Vec<String>,
    },
    Button(fn(&mut engine::Engine)),
    String {
        value: String,
        default: String,
    },
}
