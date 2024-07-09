pub mod commands;
pub mod engine;
pub mod game;
pub mod search;
pub mod evaluation;
pub mod move_generation;
pub mod various;
pub mod messages;

// use std::ffi::;
use std::os::raw::{c_char, c_void};
use std::ptr;

struct MuhleIntelligence {
    engine: engine::Engine,
    outgoing_messages: Vec<String>,
}

type CtxHandle = *mut c_void;

pub extern "C" fn muhle_intelligence_initialize() -> CtxHandle {
    let ctx = Box::new(
        MuhleIntelligence {
            engine: engine::Engine::new(write_message),
            outgoing_messages: Vec::new(),
        }
    );

    Box::leak(ctx) as *mut MuhleIntelligence as CtxHandle
}

pub extern "C" fn muhle_intelligence_uninitialize(ctx: CtxHandle) {
    unsafe {
        drop(Box::from_raw(ctx as *mut MuhleIntelligence));
    }
}

pub extern "C" fn muhle_intelligence_send(ctx: CtxHandle, string: *const c_char) {
    let ctx = ctx as *mut MuhleIntelligence;

}

pub extern "C" fn muhle_intelligence_receive(ctx: CtxHandle, string: *mut c_char) {
    let ctx = ctx as *mut MuhleIntelligence;

}

fn write_message(buffer: String) -> Result<(), String> {
    todo!()
}
