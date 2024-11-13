use anyhow::anyhow;
use ffi::update_last_error;
use http_err::HttpErrorKind;

pub struct RespBody {
    pub(crate) inner: Vec<u8>,
}

impl RespBody {
    pub fn new(data: Vec<u8>) -> Self {
        Self { inner: data }
    }
}

#[no_mangle]
pub extern "C" fn free_resp_body(handle: *mut RespBody) {
    if handle.is_null() {
        return;
    }

    unsafe {
        drop(Box::from_raw(handle));
    }
}

#[no_mangle]
pub extern "C" fn resp_body_content_len(handle: *mut RespBody) -> u64 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("resp body handle is null when use len".to_string()),
        );
        return 0;
    }

    let body = unsafe { Box::from_raw(handle) };
    let ret = body.inner.len();
    Box::leak(body);

    ret as u64
}

#[no_mangle]
pub extern "C" fn resp_body_content(handle: *mut RespBody) -> *const u8 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("resp body handle is null when use content".to_string()),
        );
        return std::ptr::null();
    }

    let body = unsafe { Box::from_raw(handle) };
    let ret = body.inner.as_ptr();
    Box::leak(body);

    ret
}
