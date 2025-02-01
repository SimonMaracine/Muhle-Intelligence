#![allow(unused)]

use crate::engine;

pub trait Option {
    fn display(&self) -> String;
}

pub struct Check<const DEFAULT: bool> {
    pub value: bool,
}

impl<const DEFAULT: bool> Check<DEFAULT> {
    pub fn new() -> Self {
        Self {
            value: DEFAULT,
        }
    }

    pub fn set(&mut self, value: &String) -> Result<(), String> {
        self.value = value.parse::<bool>().map_err(|err| format!("Could not parse value: {}", err))?;

        Ok(())
    }
}

impl<const DEFAULT: bool> Option for Check<DEFAULT> {
    fn display(&self) -> String {
        format!("type check default {}", DEFAULT)
    }
}

pub struct Spin<const DEFAULT: i32, const MIN: i32, const MAX: i32> {
    pub value: i32,
}

impl<const DEFAULT: i32, const MIN: i32, const MAX: i32> Spin<DEFAULT, MIN, MAX> {
    pub fn new() -> Self {
        Self {
            value: DEFAULT,
        }
    }

    pub fn set(&mut self, value: &String) -> Result<(), String> {
        let value = value.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?;

        if value < MIN || value > MAX {
            return Err(format!("Invalid option value: `{}`", value));
        }

        self.value = value;

        Ok(())
    }
}

impl<const DEFAULT: i32, const MIN: i32, const MAX: i32> Option for Spin<DEFAULT, MIN, MAX> {
    fn display(&self) -> String {
        format!("type spin default {} min {} max {}", DEFAULT, MIN, MAX)
    }
}

pub struct Combo<const N: usize> {
    pub value: String,
    pub default: &'static str,
    pub vars: [&'static str; N],
}

impl<const N: usize> Combo<N> {
    pub fn new(default: &'static str, vars: [&'static str; N]) -> Self {
        Self {
            value: String::from(default),
            default,
            vars,
        }
    }

    pub fn set(&mut self, value: &String) -> Result<(), String> {
        if let None = self.vars.iter().find(|item| *item == value) {
            return Err(format!("Invalid option value: `{}`", value));
        }

        self.value = value.clone();

        Ok(())
    }
}

impl<const N: usize> Option for Combo<N> {
    fn display(&self) -> String {
        let mut buffer = format!("type combo default {}", self.default);

        for var in self.vars {
            buffer += format!(" var {}", var).as_str();
        }

        buffer
    }
}

pub struct Button {
    pub func: fn(&mut engine::Engine),
}

impl Button {
    pub fn new(func: fn(&mut engine::Engine)) -> Self {
        Self {
            func,
        }
    }

    pub fn set(&mut self, engine: &mut engine::Engine) {
        (self.func)(engine);
    }
}

impl Option for Button {
    fn display(&self) -> String {
        String::from("type button")
    }
}

pub struct String_ {
    pub value: String,
    pub default: &'static str,
}

impl String_ {
    pub fn new(default: &'static str) -> Self {
        Self {
            value: String::from(default),
            default,
        }
    }

    pub fn set(&mut self, value: &String) {
        self.value = value.clone();
    }
}

impl Option for String_ {
    fn display(&self) -> String {
        format!("type string default {}", self.default)
    }
}
