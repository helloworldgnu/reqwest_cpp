use reqwest::blocking::Request;

#[no_mangle]
pub unsafe extern "C" fn request_destroy(handle: *mut Request) {
    if handle.is_null() {
        return;
    }

    drop(Box::from_raw(handle));
}
