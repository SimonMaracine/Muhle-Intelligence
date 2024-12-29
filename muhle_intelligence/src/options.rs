pub struct Option {
    name: String,
    value: Value,
}

pub enum Value {
    Check(bool),
    Spin {
        default: i32,
        min: i32,
        max: i32,
    },
    Combo {
        default: String,
        var: Vec<String>,
    },
    Button,
    String(String),
}
