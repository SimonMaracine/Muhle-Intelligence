use std::ffi::{c_char, c_int, c_uint, CStr};
use std::ptr;
use std::sync::Mutex;
use std::collections::VecDeque;

use muhle_intelligence_core::engine;
use muhle_intelligence_core::commands;

struct MuhleIntelligence {
    engine: engine::Engine,
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

    unsafe {
        CONTEXT = Some(MuhleIntelligence {
            engine: engine::Engine::new(write_message),
            messages: Mutex::new(VecDeque::new())
        });
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

    if let None = ctx {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    let input = unsafe {
        CStr::from_ptr(string).to_string_lossy().into_owned()
    };

    let tokens = commands::tokenize_command_input(input);

    if tokens.is_empty() {
        return MUHLE_INTELLIGENCE_SUCCESS;
    }

    if tokens[0] == "quit" {
        commands::quit(&mut ctx.unwrap().engine, tokens);
        return MUHLE_INTELLIGENCE_SUCCESS;
    }

    if let Err(_err) = commands::execute_command(&mut ctx.unwrap().engine, tokens) {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_receive_size(size: *mut c_uint) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    if let None = ctx {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    match ctx.unwrap().messages.lock() {
        Ok(messages) => {
            let message = messages.front();

            if let None = message {
                unsafe {
                    *size = 0;
                }

                return MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE;
            }

            unsafe {
                *size = (message.unwrap().len() + 1) as u32;
            }
        }
        Err(_err) => {
            return MUHLE_INTELLIGENCE_ERROR;
        }
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

#[no_mangle]
pub extern "C" fn muhle_intelligence_receive(string: *mut c_char) -> c_int {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    if let None = ctx {
        return MUHLE_INTELLIGENCE_ERROR;
    }

    match ctx.unwrap().messages.lock() {
        Ok(mut messages) => {
            let message = messages.pop_front();

            if let None = message {
                return MUHLE_INTELLIGENCE_MESSAGE_UNAVAILABLE;
            }

            let length = message.as_ref().unwrap().len();

            unsafe {
                ptr::copy(message.unwrap().as_ptr(), string as *mut u8, length);
                string.add(length).write(0);
            }
        }
        Err(_err) => {
            return MUHLE_INTELLIGENCE_ERROR;
        }
    }

    MUHLE_INTELLIGENCE_SUCCESS
}

fn write_message(buffer: String) -> Result<(), String> {
    let ctx = unsafe {
        CONTEXT.as_mut()
    };

    if let None = ctx {
        return Err(String::from("Context is not created"));
    }

    match ctx.unwrap().messages.lock() {
        Ok(mut messages) => {
            messages.push_back(buffer);
        }
        Err(err) => {
            return Err(format!("Could not lock the messages mutex: {}", err));
        }
    }

    Ok(())
}
