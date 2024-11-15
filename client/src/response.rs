use crate::ffi::*;
use anyhow::anyhow;
use http_err::HttpErrorKind;
use libc::c_char;
use reqwest::header::HeaderMap;
use reqwest::Version;
use resp_body::RespBody;
use rust_string::RString;
use std::{mem, ptr};
use utils;

pub struct Response {
    pub(crate) inner: Option<reqwest::blocking::Response>,
}

impl Response {
    pub fn new(inner: Option<reqwest::blocking::Response>) -> Self {
        Self { inner }
    }
}

/// Get the response text.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// Encoding is determined from the `charset` parameter of `Content-Type` header,
/// and defaults to `utf-8` if not presented.
#[no_mangle]
pub unsafe extern "C" fn response_body_text(handle: *mut Response) -> *mut RespBody {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use text"),
        );
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    if resp.inner.is_none() {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use text".to_string()),
        );

        Box::leak(resp);
        return ptr::null_mut();
    }

    let result = resp.inner.take();
    let ret = if let Some(r) = result {
        match r.text() {
            Ok(v) => {
                let buf = RespBody::new(v.into_bytes());
                Box::into_raw(Box::new(buf))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                update_last_error(kind, anyhow!(e));

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
pub unsafe extern "C" fn response_body_text_with_charset(
    handle: *mut Response,
    default_encoding: *const c_char,
) -> *mut RespBody {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use text_with_charset"),
        );
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    let ret = if let Some(r) = resp.inner.take() {
        let r_default_encoding =
            match to_rust_str(default_encoding, "default_encoding parse to str failed") {
                Some(v) => v,
                None => {
                    update_last_error(
                        HttpErrorKind::InvalidData,
                        anyhow!("default_encoding parse to str failed".to_string()),
                    );

                    Box::leak(resp);

                    return ptr::null_mut();
                }
            };

        match r.text_with_charset(r_default_encoding) {
            Ok(v) => {
                let buffer = RespBody {
                    inner: v.into_bytes(),
                };

                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                update_last_error(kind, anyhow!(e));

                ptr::null_mut()
            }
        }
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use text_with_charset".to_string()),
        );

        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

/// Get the full response body as `Bytes`.
/// The difference from copy_to is : This fun Consumption ownership
/// Don't forget free
#[no_mangle]
pub unsafe extern "C" fn response_bytes(handle: *mut Response) -> *mut RespBody {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use bytes".to_string()),
        );
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    let buf = if let Some(r) = resp.inner.take() {
        match r.bytes() {
            Ok(b) => {
                let v = b.to_vec();
                let buffer = RespBody::new(v);

                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                update_last_error(kind, anyhow!(e));

                ptr::null_mut()
            }
        }
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use bytes".to_string()),
        );
        ptr::null_mut()
    };

    Box::leak(resp);

    buf
}

/// Get the `StatusCode` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_status(handle: *mut Response) -> i32 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use status".to_string()),
        );
        return -1;
    }

    let resp = Box::from_raw(handle);
    let status = if let Some(r) = &resp.inner {
        r.status().as_u16() as i32
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use status".to_string()),
        );
        -1
    };

    Box::leak(resp);

    status
}

/// Get the `Headers` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_headers(handle: *mut Response) -> *mut HeaderMap {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use headers".to_string()),
        );
        return ptr::null_mut();
    }

    let resp = Box::from_raw(handle);
    let ret = if let Some(r) = &resp.inner {
        let headers = r.headers().clone();

        Box::into_raw(Box::new(headers))
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use headers".to_string()),
        );

        ptr::null_mut()
    };

    Box::leak(resp);

    ret
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
pub unsafe extern "C" fn response_version(handle: *mut Response) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("response handle is null when use version".to_string()),
        );
        return ptr::null_mut();
    }

    let resp = Box::from_raw(handle);
    let ret = if let Some(r) = &resp.inner {
        let res = match r.version() {
            Version::HTTP_09 => "HTTP/0.9",
            Version::HTTP_10 => "HTTP/1.0",
            Version::HTTP_11 => "HTTP/1.1",
            Version::HTTP_2 => "HTTP/2.0",
            Version::HTTP_3 => "HTTP/3.0",
            _ => "unreachable",
        };

        Box::into_raw(Box::new(RString::new(res.to_string())))
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use version".to_string()),
        );

        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

/// Get the final `Url` of this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_url(handle: *mut Response) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("handle null when use url".to_string()),
        );
        return ptr::null_mut();
    }

    let resp = Box::from_raw(handle);
    let ret = if let Some(r) = &resp.inner {
        let res = r.url().to_string();

        Box::into_raw(Box::new(RString::new(res)))
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use url".to_string()),
        );

        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

/// Get the remote address used to get this `Response`.
#[no_mangle]
pub unsafe extern "C" fn response_remote_addr(handle: *mut Response) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("handle is null when use remote_addr".to_string()),
        );
        return ptr::null_mut();
    }

    let resp = Box::from_raw(handle);
    let ret = if let Some(r) = &resp.inner {
        let res = match r.remote_addr() {
            Some(a) => a.to_string(),
            None => {
                update_last_error(
                    HttpErrorKind::InvalidData,
                    anyhow!("response remote addr is empty".to_string()),
                );
                Box::leak(resp);
                return ptr::null_mut();
            }
        };

        Box::into_raw(Box::new(RString::new(res)))
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use remote_addr".to_string()),
        );
        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

/// Get the content-length of the response, if it is known.
#[no_mangle]
pub unsafe extern "C" fn response_content_length(handle: *mut Response) -> u64 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("handle is null when use content_length".to_string()),
        );
        return 0;
    }

    let resp = Box::from_raw(handle);
    let ret = if let Some(r) = &resp.inner {
        let res = match r.content_length() {
            Some(v) => v,
            None => {
                update_last_error(
                    HttpErrorKind::InvalidData,
                    anyhow!("response content length is empty".to_string()),
                );
                Box::leak(resp);
                return 0;
            }
        };

        res
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use content_length".to_string()),
        );

        0
    };

    Box::leak(resp);

    ret
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
pub unsafe extern "C" fn response_copy_to(handle: *mut Response) -> *mut RespBody {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("handle is null when use copy_to".to_string()),
        );
        return ptr::null_mut();
    }

    let mut resp = Box::from_raw(handle);
    let ret = if let Some(ref mut r) = resp.inner {
        let mut buf: Vec<u8> = vec![];
        match r.copy_to(&mut buf) {
            Ok(_) => {
                let buffer = RespBody { inner: buf };
                Box::into_raw(Box::new(buffer))
            }
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_err(&e, &mut kind);

                update_last_error(kind, anyhow!(e));

                ptr::null_mut()
            }
        }
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use copy_to".to_string()),
        );
        ptr::null_mut()
    };

    Box::leak(resp);

    ret
}

#[no_mangle]
pub unsafe extern "C" fn response_read(handle: *mut Response, buf: *mut u8, buf_len: u32) -> i32 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("handle is null when use read".to_string()),
        );
        return -1;
    }

    use std::io::Read;

    let mut resp = Box::from_raw(handle);
    let ret = if let Some(r) = &mut resp.inner {
        let mut vec_buf = Vec::from_raw_parts(buf, buf_len as usize, buf_len as usize);
        let result = r.read(&mut vec_buf[..]);

        mem::forget(vec_buf);

        let bytes_read = match result {
            Ok(count) => count as i32,
            Err(e) => {
                let mut kind = HttpErrorKind::NoError;
                utils::parse_io_err(&e, &mut kind);

                update_last_error(kind, anyhow!(e.to_string()));

                if let Some(ref inner_err) = e.into_inner() {
                    if let Some(err) = inner_err.downcast_ref::<reqwest::Error>() {
                        let mut kind = HttpErrorKind::NoError;
                        utils::parse_err(&err, &mut kind);

                        update_last_error(kind, anyhow!(err.to_string()));
                    } else if let Some(err) = inner_err.downcast_ref::<std::io::Error>() {
                        let mut kind = HttpErrorKind::NoError;
                        utils::parse_io_err(&err, &mut kind);

                        update_last_error(kind, anyhow!(err.to_string()));
                    } else {
                        update_last_error(HttpErrorKind::Other, anyhow!(inner_err.to_string()));
                    }
                }

                -1
            }
        };

        bytes_read
    } else {
        update_last_error(
            HttpErrorKind::InvalidData,
            anyhow!("response is null when use read".to_string()),
        );

        -1
    };

    Box::leak(resp);

    ret
}

#[no_mangle]
pub unsafe extern "C" fn response_destroy(handle: *mut Response) {
    if handle.is_null() {
        return;
    }
    drop(Box::from_raw(handle))
}
