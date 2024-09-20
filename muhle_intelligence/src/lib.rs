use std::ffi::{c_char, c_int, c_uint, CStr};
use std::ptr;
use std::sync::Mutex;
use std::collections::VecDeque;

use muhle_intelligence_core::engine;
use muhle_intelligence_core::commands;

struct MuhleIntelligence {
    engine: engine::Engine,
    // The engine is free to send messages from multiple threads, hence the mutex
    messages: Mutex<VecDeque<String>>,
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
        engine: engine::Engine::new(write_to_queue),
        messages: Mutex::new(VecDeque::new()),
    };

    let ctx = unsafe {
        CONTEXT = Some(ctx);
        CONTEXT.as_ref().unwrap()
    };

    // Do this even though it's not that necessary for a library
    if let Err(_err) = ctx.engine.ready() {
        // Don't forget to reset
        unsafe {
            CONTEXT = None;
        }

        return MUHLE_INTELLIGENCE_ERROR;
    }

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

    let messages = match ctx {
        Some(ctx) => &ctx.messages,
        None => return MUHLE_INTELLIGENCE_ERROR,
    };

    match messages.lock() {
        Ok(messages) => {
            match messages.front() {
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
        }
        Err(_err) => return MUHLE_INTELLIGENCE_ERROR,
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_receive(string: *mut c_char) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    let message = match ctx {
        Some(ctx) => read_from_queue(ctx),
        None => return MUHLE_INTELLIGENCE_ERROR,
    };

    let message = match message {
        Ok(message) => message,
        Err(_err) => return MUHLE_INTELLIGENCE_ERROR,
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

fn write_to_queue(buffer: String) -> Result<(), String> {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    match ctx {
        Some(ctx) => {
            match ctx.messages.lock() {
                Ok(mut messages) => messages.push_back(buffer),
                Err(err) => return Err(format!("Could not lock the messages mutex: {}", err)),
            }
        }
        None => return Err(String::from("Context is not created"))
    }

    Ok(())
}

fn read_from_queue(ctx: &mut MuhleIntelligence) -> Result<Option<String>, String> {
    match ctx.messages.lock() {
        Ok(mut messages) => Ok(messages.pop_front()),
        Err(err) => Err(format!("Could not lock the messages mutex: {}", err)),
    }
}
