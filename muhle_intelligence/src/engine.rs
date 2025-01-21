use std::thread::{self, JoinHandle};
use std::sync::{Arc, Condvar, Mutex};
use std::sync::atomic::{AtomicBool, Ordering};

use crate::think;
use crate::game;
use crate::options;
use crate::messages;

const NAME: &str = "Muhle Intelligence 1.0";
const AUTHOR: &str = "Simon Maracine";

struct Sync {
    mutex_go: Mutex<bool>,
    cv: Condvar,
}

struct Options {
    hash: options::Option,
}

impl Options {
    fn new() -> Self {
        Self {
            hash: options::Option::Spin { value: 128, default: 128, min: 1, max: 1024 }
        }
    }
}

pub struct Engine {
    gbgp_mode: bool,
    debug_mode: bool,
    game: Arc<Mutex<game::Game>>,
    options: Options,
    handle: Option<JoinHandle<()>>,
    sync: Arc<Sync>,
    running: Arc<AtomicBool>,
    should_stop: Arc<AtomicBool>,
}

impl Engine {
    pub fn new() -> Self {
        Self {
            gbgp_mode: false,
            debug_mode: false,
            game: Arc::new(Mutex::new(game::Game {
                position: game::Position::default(),
                moves: Vec::new(),
                ponder: false,
                wtime: None,
                btime: None,
                depth: None,
                movetime: None,
            })),
            options: Options::new(),
            handle: None,
            sync: Arc::new(Sync {
                mutex_go: Mutex::new(false),
                cv: Condvar::new(),
            }),
            running: Arc::new(AtomicBool::new(false)),
            should_stop: Arc::new(AtomicBool::new(false)),
        }
    }

    pub fn is_gbgp_mode(&self) -> bool {
        self.gbgp_mode
    }

    pub fn is_debug_mode(&self) -> bool {
        self.debug_mode
    }

    pub fn gbgp(&mut self) -> Result<(), String> {
        messages::id(messages::Identifier::Name(String::from(NAME)))?;
        messages::id(messages::Identifier::Author(String::from(AUTHOR)))?;

        messages::gbgpok()?;

        self.gbgp_mode = true;

        Ok(())
    }

    pub fn debug(&mut self, active: bool) {
        self.debug_mode = active;
    }

    pub fn isready(&self) -> Result<(), String> {
        messages::readyok()
    }

    pub fn setoption(&mut self, name: &String, value: Option<&String>) -> Result<(), String> {
        match name.as_str() {
            "hash" => {
                match self.options.hash {
                    options::Option::Button(func) => func(self),
                    _ => Self::set_option_value(&mut self.options.hash, value)?,
                }
            }
            invalid => return Err(format!("Invalid option: `{}`", invalid))
        }

        Ok(())
    }

    pub fn newgame(&mut self) {
        self.initialize_once();
    }

    pub fn position(&mut self, position: game::Position, moves: Option<Vec<game::Move>>) {
        let mut game = self.game.lock().unwrap();

        game.position = position;

        if let Some(moves) = moves {
            game.moves = moves;
        } else {
            game.moves.clear();
        }
    }

    pub fn go(
        &mut self,
        ponder: bool,
        wtime: Option<u32>,
        btime: Option<u32>,
        depth: Option<i32>,
        movetime: Option<u32>,
    ) -> Result<(), String> {
        if !self.running.load(Ordering::SeqCst) {
            return Err(String::from("The engine has not been initialized"));
        }

        // Reset parameters
        {
            let mut game = self.game.lock().unwrap();

            game.ponder = ponder;
            game.wtime = wtime;
            game.btime = btime;
            game.depth = depth;
            game.movetime = movetime;
        }

        // Seems a good place to reset the stop flag
        self.should_stop.store(false, Ordering::SeqCst);

        // Trigger the think function
        {
            let mut guard = self.sync.mutex_go.lock().unwrap();
            *guard = true;
        }
        self.sync.cv.notify_one();

        Ok(())
    }

    pub fn stop(&mut self) {
        self.should_stop.store(true, Ordering::SeqCst);
    }

    pub fn ponderhit(&mut self) {

    }

    pub fn quit(&mut self) {
        if !self.running.load(Ordering::SeqCst) {
            return;
        }

        self.should_stop.store(true, Ordering::SeqCst);
        self.running.store(false, Ordering::SeqCst);

        {
            let mut guard = self.sync.mutex_go.lock().unwrap();
            *guard = true;
        }
        self.sync.cv.notify_one();

        self.handle.take().expect("The thread should have been created or this code skipped").join().unwrap();
    }

    fn initialize_once(&mut self) {
        if self.running.load(Ordering::SeqCst) {
            return;
        }

        self.running.store(true, Ordering::SeqCst);

        let sync = self.sync.clone();
        let game = self.game.clone();
        let running = self.running.clone();
        let should_stop = self.should_stop.clone();

        self.handle = Some(thread::spawn(move || {
            loop {
                let mut guard = sync.cv.wait_while(sync.mutex_go.lock().unwrap(), |go| !*go).unwrap();

                if !running.load(Ordering::SeqCst) {
                    break;
                }

                let game = game.lock().unwrap();

                let best_move = Self::think(game.clone(), should_stop.clone());

                messages::bestmove(best_move.as_ref(), None).expect("Should it fail, it's game over");

                *guard = false;
            }
        }));
    }

    fn think(game: game::Game, should_stop: Arc<AtomicBool>) -> Option<game::Move> {
        let mut think = think::Think::new();
        let ctx = think::ThinkContext::new(should_stop);

        think.think(game, ctx)
    }

    fn set_option_value(option: &mut options::Option, value_: Option<&String>) -> Result<(), String> {
        let value_ = value_.ok_or(String::from("Expected option value"))?;

        match option {
            options::Option::Check { value, .. } => {
                *value = value_.parse::<bool>().map_err(|err| format!("Could not parse value: {}", err))?;
            }
            options::Option::Spin { value, min, max, .. } => {
                let new_value = value_.parse::<i32>().map_err(|err| format!("Could not parse value: {}", err))?;

                if new_value < *min || new_value > *max {
                    return Err(format!("Invalid option value: `{}`", new_value));
                }

                *value = new_value;
            }
            options::Option::Combo { value, vars, .. } => {
                if let None = vars.iter().find(|item| *item == value_) {
                    return Err(format!("Invalid option value: `{}`", value_));
                }

                *value = value_.clone();
            }
            options::Option::String { value, .. } => {
                *value = value_.clone();
            }
            _ => debug_assert!(false)
        }

        Ok(())
    }
}
