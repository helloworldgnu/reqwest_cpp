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