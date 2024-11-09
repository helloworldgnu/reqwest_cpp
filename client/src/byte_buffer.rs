pub struct ByteBuffer {
    pub(crate) inner: Vec<u8>,
}

#[no_mangle]
pub extern "C" fn free_byte_buffer(buf: *mut ByteBuffer) {
    if buf.is_null() {
        return;
    }

    unsafe {
        drop(Box::from_raw(buf));
    }
}

#[no_mangle]
pub extern "C" fn bytes_len(data: *mut ByteBuffer) -> u64 {
    if data.is_null() {
        return 0;
    }

    unsafe { (*data).inner.len() as u64 }
}

#[no_mangle]
pub extern "C" fn bytes_content(data: *mut ByteBuffer) -> *const u8 {
    if data.is_null() {
        return std::ptr::null();
    }

    unsafe { (*data).inner.as_ptr() }
}
