use ffi::take_last_error;
use http_err::HttpErrorKind;
use std::ptr;

pub struct HttpException {
    pub(crate) error_kind: HttpErrorKind,
    pub(crate) err_msg: String,
}

impl HttpException {
    pub fn new(error_kind: HttpErrorKind, err_msg: String) -> Self {
        Self {
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
pub extern "C" fn take_last_http_error() -> *mut HttpException {
    match take_last_error() {
        None => ptr::null_mut(),
        Some(v) => Box::into_raw(v),
    }
}

#[no_mangle]
pub extern "C" fn http_err_destroy(handle: *mut HttpException) {
    if !handle.is_null() {
        unsafe {
            drop(Box::from_raw(handle));
        }
    }
}

#[no_mangle]
pub extern "C" fn http_err_kind(handle: *mut HttpException) -> HttpErrorKind {
    if handle.is_null() {
        return HttpErrorKind::HttpHandleNull;
    }

    let err = unsafe { Box::from_raw(handle) };

    let ret = err.error_kind;
    Box::leak(err);

    ret
}

#[no_mangle]
pub extern "C" fn http_err_msg_len(handle: *mut HttpException) -> u64 {
    if handle.is_null() {
        return 0;
    }

    let err = unsafe { Box::from_raw(handle) };

    let ret = err.err_msg.len();

    Box::leak(err);

    ret as u64
}

#[no_mangle]
pub extern "C" fn http_err_msg(handle: *mut HttpException) -> *const u8 {
    if handle.is_null() {
        return ptr::null();
    }

    let err = unsafe { Box::from_raw(handle) };

    let ret = err.err_msg.as_ptr();

    Box::leak(err);

    ret
}

#[no_mangle]
pub extern "C" fn http_err_clear(handle: *mut HttpException) {
    if handle.is_null() {
        return;
    }

    let mut err = unsafe { Box::from_raw(handle) };

    err.clear_error();

    Box::leak(err);
}
