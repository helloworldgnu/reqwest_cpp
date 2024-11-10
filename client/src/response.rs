use anyhow::{anyhow};
use libc::c_char;
use reqwest::header::HeaderMap;
use reqwest::Version;
use std::ffi::{CString};
use std::{mem, ptr};
use byte_buffer::ByteBuffer;
use http_err::HttpErrorKind;
use crate::ffi::*;

use utils;

pub struct Response {
    pub(crate) inner: Option<reqwest::blocking::Response>,

    pub(crate) error_kind: HttpErrorKind,
    pub(crate) err_msg: String,
}

impl Response {
    pub fn new(inner: Option<reqwest::blocking::Response>, error_kind: HttpErrorKind, err_msg: String) -> Self {
        Self {
            inner,

            error_kind,
            err_msg,
        }
    }

    pub fn clear_error(&mut self) {
        self.error_kind = HttpErrorKind::NoError;
        self.err_msg = String::default();
    }
}

#[no_mangle]
pub extern "C" fn resp_err_kind(handle: *mut Response) -> HttpErrorKind {
    if handle.is_null() {
        return HttpErrorKind::InvalidInput;
    }

    let resp = unsafe {
        Box::from_raw(handle)
    };

    let ret = resp.error_kind;
    Box::leak(resp);

    ret
}

#[no_mangle]
pub extern "C" fn resp_err_msg_len(handle: *mut Response) -> u64 {
    if handle.is_null() {
        return 0;
    }

    let resp = unsafe {
        Box::from_raw(handle)
    };

    let ret = resp.err_msg.len();

    Box::leak(resp);

    ret as u64
}

#[no_mangle]
pub extern "C" fn resp_err_msg(handle: *mut Response) -> *const u8 {
    if handle.is_null() {
        return ptr::null();
    }

    let resp = unsafe {
        Box::from_raw(handle)
    };

    let ret = resp.err_msg.as_ptr();

    Box::leak(resp);

    ret
}

#[no_mangle]
pub extern "C" fn resp_err_clear(handle: *mut Response) {
    if handle.is_null() {
        return;
    }

    let mut resp = unsafe {
        Box::from_raw(handle)
    };

    resp.clear_error();

    Box::leak(resp);
}

/// Get the response text.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// Encoding is determined from the `charset` parameter of `Content-Type` header,
/// and defaults to `utf-8` if not presented.
#[no_mangle]
pub unsafe extern "C" fn response_text(
    handle: *mut Response,
) -> *mut ByteBuffer {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use text"));
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    if resp.inner.is_none() {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use text".to_string();

        Box::leak(resp);
        return ptr::null_mut();
    }

    let result = resp.inner.take();
    let ret = if let Some(r) = result {
        match r.text() {
            Ok(v) => {
                let buf = ByteBuffer::new(v.into_bytes());
                Box::into_raw(Box::new(buf))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                resp.error_kind = kind;
                resp.err_msg = e.to_string();

                ptr::null_mut()
            }
        }
    } else {
        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

/// Get the response text given a specific encoding.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// You can provide a default encoding for decoding the raw message, while the
/// `charset` parameter of `Content-Type` header is still prioritized. For more information
/// about the possible encoding name, please go to [`encoding_rs`] docs.
///
/// [`encoding_rs`]: https://docs.rs/encoding_rs/0.8/encoding_rs/#relationship-with-windows-code-pages
#[no_mangle]
pub unsafe extern "C" fn response_text_with_charset(
    handle: *mut Response,
    default_encoding: *const c_char,
) -> *mut ByteBuffer {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use text_with_charset"));
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = resp.inner.take() {
        let r_default_encoding =
            match to_rust_str(default_encoding, "default_encoding parse to str failed") {
                Some(v) => v,
                None => {
                    resp.error_kind = HttpErrorKind::InvalidData;
                    resp.err_msg = "default_encoding parse to str failed".to_string();

                    Box::leak(resp);

                    return ptr::null_mut();
                }
            };

        match r.text_with_charset(r_default_encoding) {
            Ok(v) => {
                let buffer = ByteBuffer { inner: v.into_bytes() };
                Box::leak(resp);

                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                resp.error_kind = kind;
                resp.err_msg = e.to_string();

                Box::leak(resp);

                ptr::null_mut()
            }
        }
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use text_with_charset".to_string();

        Box::leak(resp);

        ptr::null_mut()
    }
}

/// Get the full response body as `Bytes`.
/// The difference from copy_to is : This fun Consumption ownership
/// Don't forget free
#[no_mangle]
pub unsafe extern "C" fn response_bytes(
    handle: *mut Response,
) -> *mut ByteBuffer {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use bytes"));
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    let buf = if let Some(r) = resp.inner.take() {
        match r.bytes() {
            Ok(b) => {
                let v = b.to_vec();
                let buffer = ByteBuffer::new(v);

                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                resp.error_kind = kind;
                resp.err_msg = e.to_string();

                ptr::null_mut()
            }
        }
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use bytes".to_string();

        ptr::null_mut()
    };

    Box::leak(resp);

    buf
}

/// Get the `StatusCode` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_status(handle: *mut Response) -> i32 {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use status"));
        return -1;
    }

    let mut resp = Box::from_raw(handle);
    let status = if let Some(r) = &resp.inner {
        r.status().as_u16() as i32
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use status".to_string();

        -1
    };

    Box::leak(resp);

    status
}

/// Get the `Headers` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_headers(handle: *mut Response) -> *mut HeaderMap {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use headers"));
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = &resp.inner {
        let headers = r.headers().clone();
        Box::leak(resp);

        Box::into_raw(Box::new(headers))
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use headers".to_string();

        Box::leak(resp);

        ptr::null_mut()
    }
}

/// Get the HTTP `Version` of this `Response`.
/// Don't forget free string
///Version::HTTP_09 => "HTTP/0.9",
///Version::HTTP_10 => "HTTP/1.0",
///Version::HTTP_11 => "HTTP/1.1",
///Version::HTTP_2 => "HTTP/2.0",
///Version::HTTP_3 => "HTTP/3.0",
///_ => "unreachable"
#[no_mangle]
pub unsafe extern "C" fn response_version(handle: *mut Response) -> *const c_char {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use version"));
        return ptr::null();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = &resp.inner {
        let res = match r.version() {
            Version::HTTP_09 => "HTTP/0.9",
            Version::HTTP_10 => "HTTP/1.0",
            Version::HTTP_11 => "HTTP/1.1",
            Version::HTTP_2 => "HTTP/2.0",
            Version::HTTP_3 => "HTTP/3.0",
            _ => "unreachable",
        };

        let ret = match CString::new(res) {
            Ok(v) => { v.into_raw() }
            Err(e) => {
                resp.error_kind = HttpErrorKind::OutOfMemory;
                resp.err_msg = e.to_string();

                ptr::null()
            }
        };

        Box::leak(resp);

        ret
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use version".to_string();

        Box::leak(resp);

        ptr::null()
    }
}

/// Get the final `Url` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_url(handle: *mut Response) -> *const c_char {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use url"));
        return ptr::null();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = &resp.inner {
        let res = r.url().to_string();

        let ret = match CString::new(res) {
            Ok(v) => { v.into_raw() }
            Err(e) => {
                resp.error_kind = HttpErrorKind::OutOfMemory;
                resp.err_msg = e.to_string();

                ptr::null()
            }
        };

        Box::leak(resp);

        ret
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use headers".to_string();

        Box::leak(resp);

        ptr::null()
    }
}

/// Get the remote address used to get this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_remote_addr(handle: *mut Response) -> *const c_char {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use remote_addr"));
        return ptr::null();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = &resp.inner {
        let res = match r.remote_addr() {
            Some(a) => a.to_string(),
            None => {
                resp.error_kind = HttpErrorKind::InvalidData;
                resp.err_msg = "response remote addr is empty".to_string();

                Box::leak(resp);
                return ptr::null();
            }
        };

        let ret = match CString::new(res) {
            Ok(v) => { v.into_raw() }
            Err(e) => {
                resp.error_kind = HttpErrorKind::OutOfMemory;
                resp.err_msg = e.to_string();

                ptr::null()
            }
        };

        Box::leak(resp);

        ret
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use remote_addr".to_string();

        Box::leak(resp);

        ptr::null()
    }
}

/// Get the content-length of the response, if it is known.
#[no_mangle]
pub unsafe extern "C" fn response_content_length(handle: *mut Response) -> u64 {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use content_length"));
        return 0;
    }

    let mut resp = Box::from_raw(handle);
    if let Some(r) = &resp.inner {
        let res = match r.content_length() {
            Some(v) => v,
            None => {
                resp.error_kind = HttpErrorKind::InvalidData;
                resp.err_msg = "response content length is empty".to_string();

                Box::leak(resp);
                return 0;
            }
        };

        Box::leak(resp);

        res
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use content_length".to_string();

        Box::leak(resp);

        0
    }
}

/// Copy the response body into a writer.
/// Don't forget free
///
/// This function internally uses [`std::io::copy`] and hence will continuously read data from
/// the body and then write it into writer in a streaming fashion until EOF is met.
///
/// On success, the total number of bytes that were copied to `writer` is returned.
///
/// [`std::io::copy`]: https://doc.rust-lang.org/std/io/fn.copy.html
#[no_mangle]
pub unsafe extern "C" fn response_copy_to(handle: *mut Response) -> *mut ByteBuffer {
    if handle.is_null() {
        update_last_error(anyhow!("response is null when use copy_to"));
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    if let Some(ref mut r) = resp.inner {
        let mut buf: Vec<u8> = vec![];
        match r.copy_to(&mut buf) {
            Ok(_) => {
                let buffer = ByteBuffer { inner: buf };

                Box::leak(resp);

                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                resp.error_kind = kind;
                resp.err_msg = e.to_string();

                Box::leak(resp);

                ptr::null_mut()
            }
        }
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use copy_to".to_string();

        Box::leak(resp);

        ptr::null_mut()
    }
}

#[no_mangle]
pub unsafe extern "C" fn response_read(
    response: *mut Response,
    buf: *mut u8,
    buf_len: u32,
) -> i32 {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use copy"));
        return -1;
    }

    use std::io::Read;

    let mut resp = Box::from_raw(response);
    if let Some(r) = &mut resp.inner {
        let mut vec_buf = Vec::from_raw_parts(buf, buf_len as usize, buf_len as usize);
        let result = r.read(&mut vec_buf[..]);

        mem::forget(vec_buf);

        let bytes_read = match result {
            Ok(count) => count as i32,
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_io_err(&e, &mut kind);

                resp.error_kind = kind;
                resp.err_msg = e.to_string();

                if let Some(ref inner_err) = e.into_inner() {
                    if let Some(err) = inner_err.downcast_ref::<reqwest::Error>() {
                        let mut kind = HttpErrorKind::NoError;
                        utils::parse_err(&err, &mut kind);

                        resp.error_kind = kind;
                    } else if let Some(err) = inner_err.downcast_ref::<std::io::Error>() {
                        let mut kind = HttpErrorKind::NoError;
                        utils::parse_io_err(&err, &mut kind);

                        resp.error_kind = kind;
                    }

                    resp.err_msg = inner_err.to_string();
                }

                -1
            }
        };

        Box::leak(resp);

        bytes_read
    } else {
        resp.error_kind = HttpErrorKind::InvalidData;
        resp.err_msg = "response is null when use read".to_string();

        Box::leak(resp);

        -1
    }
}

#[no_mangle]
pub unsafe extern "C" fn response_destroy(handle: *mut Response) {
    if handle.is_null() {
        return;
    }
    drop(Box::from_raw(handle))
}
