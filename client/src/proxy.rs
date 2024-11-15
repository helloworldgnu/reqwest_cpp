use crate::ffi::*;
use anyhow::{anyhow, Error};
use http_err::HttpErrorKind;
use libc::c_char;
use std::{ffi::CStr, ptr};

/// Proxy all HTTP traffic to the passed URL.
#[no_mangle]
pub unsafe extern "C" fn proxy_http(proxy_scheme: *const c_char) -> *mut reqwest::Proxy {
    if proxy_scheme.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("proxy_scheme is null"),
        );
        return ptr::null_mut();
    }

    let scheme = if let Ok(proxy_scheme) = CStr::from_ptr(proxy_scheme).to_str() {
        proxy_scheme
    } else {
        return ptr::null_mut();
    };

    let result = reqwest::Proxy::http(scheme);
    match result {
        Ok(p) => Box::into_raw(Box::new(p)),
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e).context("proxy fail"));
            ptr::null_mut()
        }
    }
}

/// Proxy all HTTPS traffic to the passed URL.
#[no_mangle]
pub unsafe extern "C" fn proxy_https(proxy_scheme: *const c_char) -> *mut reqwest::Proxy {
    if proxy_scheme.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("proxy_scheme is null"),
        );
        return ptr::null_mut();
    }

    let scheme = if let Ok(proxy_scheme) = CStr::from_ptr(proxy_scheme).to_str() {
        proxy_scheme
    } else {
        return ptr::null_mut();
    };

    let result = reqwest::Proxy::https(scheme);
    match result {
        Ok(p) => Box::into_raw(Box::new(p)),
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e).context("proxy fail"));
            ptr::null_mut()
        }
    }
}

/// Proxy **all** traffic to the passed URL.
#[no_mangle]
pub unsafe extern "C" fn proxy_all(proxy_scheme: *const c_char) -> *mut reqwest::Proxy {
    if proxy_scheme.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("proxy_scheme is null"),
        );
        return ptr::null_mut();
    }

    let scheme = if let Ok(proxy_scheme) = CStr::from_ptr(proxy_scheme).to_str() {
        proxy_scheme
    } else {
        return ptr::null_mut();
    };

    let result = reqwest::Proxy::all(scheme);
    match result {
        Ok(p) => Box::into_raw(Box::new(p)),
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e).context("proxy fail"));
            ptr::null_mut()
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn proxy_destroy(handle: *mut reqwest::Proxy) {
    if handle.is_null() {
        return;
    }
    drop(Box::from_raw(handle));
}
