pub struct RString {
    inner: String,
}

impl RString {
    pub fn new(s: String) -> Self {
        Self { inner: s }
    }

    pub fn data_len(&self) -> usize {
        self.inner.len()
    }

    pub fn data_ptr(&self) -> *const u8 {
        self.inner.as_ptr()
    }
}

#[no_mangle]
pub extern "C" fn free_r_string(handle: *mut RString) {
    if handle.is_null() {
        return;
    }

    unsafe {
        drop(Box::from_raw(handle));
    }
}

#[no_mangle]
pub extern "C" fn r_string_len(handle: *mut RString) -> u64 {
    if handle.is_null() {
        return 0;
    }

    let r_string = unsafe { Box::from_raw(handle) };
    let ret = r_string.data_len();
    Box::leak(r_string);

    ret as u64
}

#[no_mangle]
pub extern "C" fn r_string_bytes(handle: *mut RString) -> *const u8 {
    if handle.is_null() {
        return std::ptr::null();
    }

    let r_string = unsafe { Box::from_raw(handle) };
    let ret = r_string.data_ptr();
    Box::leak(r_string);

    ret
}
