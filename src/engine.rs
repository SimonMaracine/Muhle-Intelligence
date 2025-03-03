use std::thread::{self, JoinHandle};
use std::sync::{Arc, Condvar, Mutex};
use std::sync::atomic::{AtomicBool, Ordering};

use crate::think;
use crate::game;
use crate::options;
use crate::messages;

const NAME: &str = "Muhle Intelligence 1.0-alpha";
const AUTHOR: &str = "Simon M.";

struct Sync {
    mutex_go: Mutex<bool>,
    cv: Condvar,
}

struct Options {
    twelve_mens_morris: options::Check<false>,
}

impl Options {
    fn new() -> Self {
        Self {
            twelve_mens_morris: options::Check::new(),
        }
    }
}

pub struct Engine {
    gbgp_mode: bool,
    debug_mode: bool,
    game: Arc<Mutex<game::Game>>,
    options: Arc<Mutex<Options>>,
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
            options: Arc::new(Mutex::new(Options::new())),
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

        {
            let options = self.options.lock().unwrap();
            messages::option(String::from("TwelveMensMorris"), &options.twelve_mens_morris)?;
        }

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
        let mut options = self.options.lock().unwrap();

        match name.as_str() {
            "TwelveMensMorris" => {
                if let Some(value) = value {
                    options.twelve_mens_morris.set(&value)?;
                } else {
                    return Err(format!("Expected value for option"));
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

        self.handle.take().expect("The thread should have been created").join().unwrap();
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
        let options = self.options.clone();

        self.handle = Some(thread::spawn(move || {
            loop {
                let mut guard = sync.cv.wait_while(sync.mutex_go.lock().unwrap(), |go| !*go).unwrap();

                if !running.load(Ordering::SeqCst) {
                    break;
                }

                let twelve_mens_morris = {
                    options.lock().unwrap().twelve_mens_morris.value
                };

                let game = game.lock().unwrap();

                let best_move = Self::think(game.clone(), should_stop.clone(), twelve_mens_morris);

                messages::bestmove(best_move.as_ref(), None).expect("Should it fail, it's game over");

                *guard = false;
            }
        }));
    }

    fn think(game: game::Game, should_stop: Arc<AtomicBool>, twelve_mens_morris: bool) -> Option<game::Move> {
        let mut think = think::Think::new();
        let ctx = think::ThinkContext::new(should_stop);

        if twelve_mens_morris {
            think.think::<{game::TWELVE}>(game, ctx)
        } else {
            think.think::<{game::NINE}>(game, ctx)
        }
    }
}
