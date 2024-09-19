use std::ffi::{c_char, c_int, c_uint, CStr};
use std::ptr;
use std::collections::VecDeque;

use muhle_intelligence_core::engine;
use muhle_intelligence_core::commands;

struct MuhleIntelligence {
    engine: engine::Engine,
    messages: VecDeque<String>,
}

static mut CONTEXT: Option<MuhleIntelligence> = None;

const MUHLE_INTELLIGENCE_ERROR: i32 = -1;
const MUHLE_INTELLIGENCE_SUCCESS: i32 = 0;
const MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE: i32 = 1;

#[no_mangle]
pub extern "C" fn muhle_intelligence_initialize() -> c_int {
    unsafe {
        if CONTEXT.is_some() {
            return MUHLE_INTELLIGENCE_ERROR;
        }
    }

    let ctx = MuhleIntelligence {
        engine: engine::Engine::new(write_message),
        messages: VecDeque::new(),
    };

    // Do this even though it's not that necessary for a library
    if let Err(_err) = ctx.engine.ready() {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    unsafe {
        CONTEXT = Some(ctx);
    };

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_uninitialize() -> c_int {
    unsafe {
        if CONTEXT.is_none() {
            return MUHLE_INTELLIGENCE_ERROR;
        }
    }

    unsafe {
        CONTEXT = None;
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_send(string: *const c_char) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    let engine = match ctx {
        Some(ctx) => &mut ctx.engine,
        None => return MUHLE_INTELLIGENCE_ERROR,
    };

    let input = unsafe {
        CStr::from_ptr(string).to_string_lossy().into_owned()
    };

    let tokens = commands::tokenize_command_input(input);

    if tokens.is_empty() {
        return MUHLE_INTELLIGENCE_SUCCESS;
    }

    if tokens[0] == "quit" {
        commands::quit(engine, tokens);
        return MUHLE_INTELLIGENCE_SUCCESS;
    }

    if let Err(_err) = commands::execute_command(engine, tokens) {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_receive_size(size: *mut c_uint) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    let message = match ctx {
        Some(ctx) => ctx.messages.pop_front(),
        None => return MUHLE_INTELLIGENCE_ERROR,
    };

    match message {
        Some(message) => {
            unsafe {
                *size = message.len() as u32;
            }
        }
        None => {
            unsafe {
                *size = 0;
            }

            return MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE;
        }
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_receive(string: *mut c_char) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    let message = match ctx {
        Some(ctx) => ctx.messages.pop_front(),
        None => return MUHLE_INTELLIGENCE_ERROR,
    };

    match message {
        Some(message) => {
            unsafe {
                ptr::copy(message.as_ptr(), string as *mut u8, message.len());
            }
        }
        None => return MUHLE_INTELLIGENCE_ERROR  // Users must not call muhle_intelligence_receive willy-nilly
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

fn write_message(buffer: String) -> Result<(), String> {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    match ctx {
        Some(ctx) => ctx.messages.push_back(buffer),
        None => return Err(String::from("Context is not created")),
    }

    Ok(())
}
