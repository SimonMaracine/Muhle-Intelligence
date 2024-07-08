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
use std::alloc;

struct MuhleIntelligence {
    engine: engine::Engine,
    incoming_messages: Vec<String>,
    outgoing_messages: Vec<String>,
}

type CtxHandle = *mut c_void;

pub extern "C" fn muhle_intelligence_initialize() -> CtxHandle {
    let layout = alloc::Layout::new::<MuhleIntelligence>();

    let ctx = unsafe {
        alloc::alloc(layout)
    };

    if ctx.is_null() {
        return ptr::null_mut();
    }

    ctx as CtxHandle
}

pub extern "C" fn muhle_intelligence_uninitialize(ctx: CtxHandle) {
    let layout = alloc::Layout::new::<MuhleIntelligence>();

    unsafe {
        alloc::dealloc(ctx as *mut u8, layout)
    }
}

pub extern "C" fn muhle_intelligence_send(ctx: CtxHandle, string: *const c_char) {
    let ctx = ctx as *mut MuhleIntelligence;

}

pub extern "C" fn muhle_intelligence_receive(ctx: CtxHandle, string: *mut c_char) {
    let ctx = ctx as *mut MuhleIntelligence;

}
