use std::sync::atomic::{AtomicBool, Ordering};
use std::thread::{self, JoinHandle};
use std::sync::{Arc, Condvar, Mutex};
use std::str::FromStr;

use crate::evaluation;
use crate::game;
use crate::search;
use crate::options;
use crate::messages;

const NAME: &str = "Muhle Intelligence 1.0";
const AUTHOR: &str = "Simon Maracine";

#[derive(Clone)]
struct Game {
    position: game::Position,
    moves: Vec<game::Move>,
}

struct Sync {
    mutex_go: Mutex<bool>,
    cv: Condvar,
}

pub struct Engine {
    gbgp_mode: bool,
    debug_mode: bool,
    game: Arc<Mutex<Game>>,
    handle: Option<JoinHandle<()>>,
    sync: Arc<Sync>,
    running: Arc<AtomicBool>,
}

impl Engine {
    pub fn new() -> Self {
        Self {
            gbgp_mode: false,
            debug_mode: false,
            game: Arc::new(Mutex::new(Game {
                position: game::Position::default(),
                moves: Vec::new()
            })),
            handle: None,
            sync: Arc::new(Sync {
                mutex_go: Mutex::new(false),
                cv: Condvar::new()
            }),
            running: Arc::new(AtomicBool::new(false)),
        }
    }

    pub fn is_gbgp_mode(&self) -> bool {
        self.gbgp_mode
    }

    pub fn gbgp(&self) {
        messages::id(messages::Identifier::Name(String::from(NAME)));
        messages::id(messages::Identifier::Author(String::from(AUTHOR)));

        messages::gbgpok();
    }

    pub fn debug(&mut self, active: bool) {
        self.debug_mode = active;
    }

    pub fn isready(&self) {
        messages::readyok();
    }

    pub fn setoption(&mut self, name: String, value: Option<String>) {

    }

    pub fn newgame(&mut self) {
        self.initialize_once();
    }

    pub fn position(&mut self, position: game::Position, moves: Option<Vec<game::Move>>) -> Result<(), String> {
        let mut game = self.game.lock().unwrap();

        game.position = position;

        game.moves.clear();

        if let Some(moves) = moves {
            for move_ in moves {
                game.moves.push(move_);
            }
        }

        Ok(())
    }

    pub fn go(&mut self, ponder: bool, wtime: Option<i32>, btime: Option<i32>, maxdepth: Option<i32>, maxtime: Option<i32>) {
        {
            let mut guard = self.sync.mutex_go.lock().unwrap();
            *guard = true;
        }
        self.sync.cv.notify_one();
    }

    pub fn stop(&mut self) {

    }

    pub fn ponderhit(&mut self) {

    }

    pub fn quit(&mut self) {
        if !self.running.load(Ordering::SeqCst) {
            return;
        }

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

        self.handle = Some(thread::spawn(move || {
            loop {
                let _guard = sync.cv.wait_while(sync.mutex_go.lock().unwrap(), |go| *go).unwrap();

                if !running.load(Ordering::SeqCst) {
                    break;
                }

                let game = game.lock().unwrap();

                Self::think(game.clone());
            }
        }));
    }

    fn think(game: Game) {

    }
}
