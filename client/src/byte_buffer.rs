pub struct ByteBuffer {
    pub(crate) inner: Vec<u8>,
}

impl ByteBuffer {
    pub fn new(data: Vec<u8>) -> Self {
        Self {
            inner: data,
        }
    }
}

#[no_mangle]
pub extern "C" fn free_byte_buffer(handle: *mut ByteBuffer) {
    if handle.is_null() {
        return;
    }

    unsafe {
        drop(Box::from_raw(handle));
    }
}

#[no_mangle]
pub extern "C" fn bytes_len(handle: *mut ByteBuffer) -> u64 {
    if handle.is_null() {
        return 0;
    }

    unsafe { (*handle).inner.len() as u64 }
}

#[no_mangle]
pub extern "C" fn bytes_content(handle: *mut ByteBuffer) -> *const u8 {
    if handle.is_null() {
        return std::ptr::null();
    }

    unsafe { (*handle).inner.as_ptr() }
}
