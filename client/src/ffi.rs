//! The foreign function interface which exposes this library to non-Rust
//! languages.

use anyhow::Error;
use libc::{c_char, wchar_t};
use std::cell::RefCell;
use std::convert::From;
use std::ffi::{CStr, CString, OsString};
use std::{ptr, time::Duration, usize};

use http_err::HttpErrorKind;
use http_exeception::HttpException;
#[cfg(target_os = "windows")]
use std::os::windows::ffi::OsStringExt;

thread_local! {
    static LAST_ERROR : RefCell<Option<Box<HttpException>>> = RefCell::new(None);
}

#[repr(C)]
pub struct Pair {
    key: *const c_char,
    value: *const c_char,
}

impl From<(String, String)> for Pair {
    fn from(tup: (String, String)) -> Self {
        Pair {
            key: match CString::new(tup.0) {
                Ok(v) => v.into_raw(),
                Err(_) => ptr::null(),
            },
            value: match CString::new(tup.1) {
                Ok(v) => v.into_raw(),
                Err(_) => ptr::null_mut(),
            },
        }
    }
}

impl From<&Pair> for (String, String) {
    fn from(tup: &Pair) -> (String, String) {
        let key = match to_rust_str(tup.key, "key parse error") {
            Some(v) => v.to_string(),
            None => String::from(""),
        };
        let value = match to_rust_str(tup.value, "value parse error") {
            Some(v) => v.to_string(),
            None => String::from(""),
        };
        (key, value)
    }
}

/// Retrieve the most recent error, clearing it in the process.
pub fn take_last_error() -> Option<Box<HttpException>> {
    LAST_ERROR.with(|prev| prev.borrow_mut().take())
}

/// Update the most recent error, clearing whatever may have been there before.
pub fn update_last_error(kind: HttpErrorKind, err: Error) {
    error!("Setting LAST_ERROR: {}", err);
    println!("update_last_error : {}", err);

    {
        // Print a pseudo-backtrace for this error, following back each error's
        // cause until we reach the root error.
        let mut cause = err.source();
        while let Some(parent_err) = cause {
            warn!("Http error occurred, caused by: {}", parent_err);
            cause = parent_err.source();
        }
    }

    LAST_ERROR.with(|prev| {
        *prev.borrow_mut() = Some(Box::new(HttpException::new(kind, err.to_string())));
    });
}

pub fn to_rust_str<'a>(ptr: *const c_char, err_tip: &'static str) -> Option<&'a str> {
    if ptr.is_null() {
        return None;
    }

    unsafe {
        match CStr::from_ptr(ptr).to_str() {
            Ok(v) => Some(v),
            Err(e) => {
                update_last_error(
                    HttpErrorKind::CharConversion,
                    Error::new(e).context(err_tip),
                );
                None
            }
        }
    }
}

#[cfg(target_os = "windows")]
pub fn to_rust_str_wide<'a>(ptr: *const wchar_t, length: usize) -> Option<OsString> {
    if ptr.is_null() {
        return None;
    }

    let mut len = length;
    if len == 0 {
        unsafe {
            len = (0..).take_while(|&i| *ptr.offset(i) != 0).count();
        }
    }

    let path_slice = unsafe { std::slice::from_raw_parts(ptr, len) };

    Some(std::ffi::OsString::from_wide(path_slice))
}

pub fn u64_to_millis_duration(millisecond: *const u64) -> Option<Duration> {
    if millisecond.is_null() {
        return None;
    }

    unsafe { Some(Duration::from_millis(*millisecond)) }
}
