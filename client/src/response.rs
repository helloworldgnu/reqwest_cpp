use anyhow::{anyhow, Error};
use libc::c_char;
use reqwest::blocking::Response;
use reqwest::header::HeaderMap;
use reqwest::Version;
use std::ffi::CString;
use std::{mem, ptr};

use crate::ffi::*;

use utils;

/// Get the response text.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// Encoding is determined from the `charset` parameter of `Content-Type` header,
/// and defaults to `utf-8` if not presented.
#[no_mangle]
pub unsafe extern "C" fn response_text(
    response: *mut Response,
    kind: *mut u32,
    value: *mut i32,
) -> *const c_char {
    *kind = 0;
    *value = 0;

    if response.is_null() {
        *kind = 1001;
        update_last_error(anyhow!("response is null when use text"));
        return ptr::null_mut();
    }

    let result = Box::from_raw(response).text();
    match result {
        Ok(v) => CString::new(v).unwrap().into_raw(),
        Err(e) => {
            utils::parse_err(&e, kind, value);
            ptr::null_mut()
        }
    }
}

/// Get the `StatusCode` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_status(response: *mut Response) -> u16 {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use status"));
        return u16::MAX;
    }
    (*response).status().as_u16()
}

/// Get the `Headers` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_headers(response: *mut Response) -> *const HeaderMap {
    //todo
    if response.is_null() {
        update_last_error(anyhow!("response is null when use status"));
        return ptr::null();
    }

    //println!("debug response_headers");
    let headers = (*response).headers().clone();
    //println!("{:#?}",headers);
    Box::into_raw(Box::new(headers))
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
pub unsafe extern "C" fn response_version(response: *mut Response) -> *const c_char {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use version"));
        return ptr::null();
    }

    let res = match (*response).version() {
        Version::HTTP_09 => "HTTP/0.9",
        Version::HTTP_10 => "HTTP/1.0",
        Version::HTTP_11 => "HTTP/1.1",
        Version::HTTP_2 => "HTTP/2.0",
        Version::HTTP_3 => "HTTP/3.0",
        _ => "unreachable",
    };
    CString::new(res).unwrap().into_raw()
}

/// Get the final `Url` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_url(response: *mut Response) -> *const c_char {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use url"));
        return ptr::null();
    }

    let res = (*response).url().to_string();
    CString::new(res).unwrap().into_raw()
}

/// Get the remote address used to get this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_remote_addr(response: *mut Response) -> *const c_char {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use remote_addr"));
        return ptr::null();
    }

    let res = match (*response).remote_addr() {
        Some(a) => a.to_string(),
        None => {
            return ptr::null();
        }
    };
    CString::new(res).unwrap().into_raw()
}

/// todo extensions.

/// Get the content-length of the response, if it is known.
#[no_mangle]
pub unsafe extern "C" fn response_content_length(response: *mut Response) -> *const u64 {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use content_length"));
        return ptr::null();
    }

    let res = match (*response).content_length() {
        Some(v) => v,
        None => {
            return ptr::null();
        }
    };
    Box::into_raw(Box::new(res))
}

#[no_mangle]
pub unsafe extern "C" fn response_content_length_destroy(content_length: *mut u64) {
    if content_length.is_null() {
        return;
    }
    drop(Box::from_raw(content_length))
}

/// Get the full response body as `Bytes`.
/// The difference from copy_to is : This fun Consumption ownership
/// Don't forget free
#[no_mangle]
pub unsafe extern "C" fn response_bytes(
    response: *mut Response,
    kind: *mut u32,
    value: *mut i32,
) -> *const u8 {
    *kind = 0;
    *value = 0;

    if response.is_null() {
        *kind = 1001;
        update_last_error(anyhow!("response is null when use bytes"));
        return ptr::null();
    }

    let r_response = Box::from_raw(response);
    match r_response.bytes() {
        Ok(b) => {
            let v = b.to_vec();
            let v_ptr = v.as_ptr();
            mem::forget(v);
            v_ptr
        }
        Err(e) => {
            utils::parse_err(&e, kind, value);

            update_last_error(Error::new(e));
            ptr::null()
        }
    }
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
    response: *mut Response,
    default_encoding: *const c_char,
) -> *const c_char {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use bytes"));
        return ptr::null();
    }

    let r_default_encoding =
        match to_rust_str(default_encoding, "default_encoding parse to str failed") {
            Some(v) => v,
            None => {
                return ptr::null();
            }
        };
    let r_response = Box::from_raw(response);
    let res = match r_response.text_with_charset(r_default_encoding) {
        Ok(v) => v.to_string(),
        Err(e) => {
            update_last_error(Error::new(e));
            return ptr::null();
        }
    };
    CString::new(res).unwrap().into_raw()
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
pub unsafe extern "C" fn response_copy_to(response: *mut Response) -> *const u8 {
    if response.is_null() {
        update_last_error(anyhow!("response is null when use copy"));
        return ptr::null();
    }

    let mut buf: Vec<u8> = vec![];
    match (*response).copy_to(&mut buf) {
        Ok(_) => {
            let v_ptr = buf.as_ptr();
            mem::forget(buf);
            v_ptr
        }
        Err(e) => {
            update_last_error(Error::new(e));
            return ptr::null();
        }
    }
}

//  int32_t read(uint8_t *buf, uint32_t buf_len);
#[no_mangle]
pub unsafe extern "C" fn response_read(
    response: *mut Response,
    buf: *mut u8,
    buf_len: u32,
    kind: *mut u32,
) -> i32 {
    *kind = 0;

    if response.is_null() {
        *kind = 2001;
        update_last_error(anyhow!("response is null when use copy"));
        return -1;
    }

    use std::io::Read;

    let resp = &mut *response;

    let mut vec_buf = Vec::from_raw_parts(buf, buf_len as usize, buf_len as usize);
    let result = resp.read(&mut vec_buf[..]);

    std::mem::forget(vec_buf);

    let bytes_read = match result {
        Ok(count) => count as i32,
        Err(e) => {
            *kind = 2001;

            if let Some(inner_err) = e.into_inner() {
                if let Some(err) = inner_err.downcast_ref::<reqwest::Error>() {
                    utils::parse_err(err, kind, std::ptr::null_mut());
                } else if let Some(err) = inner_err.downcast_ref::<std::io::Error>() {
                    utils::parse_read_err(err, kind);
                }

                update_last_error(anyhow::anyhow!(inner_err));
            }
            -1
        }
    };

    bytes_read
}

#[no_mangle]
pub unsafe extern "C" fn response_destroy(response: *mut Response) {
    if response.is_null() {
        return;
    }
    drop(Box::from_raw(response))
}
