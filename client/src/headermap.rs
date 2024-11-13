use crate::ffi::*;
use anyhow::{anyhow, Error};
use http_err::HttpErrorKind;
use libc::c_char;
use reqwest::header::{HeaderMap, HeaderValue};
use rust_string::RString;
use std::ptr;

#[no_mangle]
pub unsafe extern "C" fn new_header_map() -> *mut HeaderMap {
    Box::into_raw(Box::new(HeaderMap::new()))
}

/// Inserts a key-value pair into the map.
///
/// If the map did not previously have this key present, then `None` is
/// returned.
///
/// If the map did have this key present, the new value is associated with
/// the key and all previous values are removed. **Note** that only a single
/// one of the previous values is returned. If there are multiple values
/// that have been previously associated with the key, then the first one is
/// returned. See `insert_mult` on `OccupiedEntry` for an API that returns
/// all values.
///
/// The key is not updated, though; this matters for types that can be `==`
/// without being identical.
#[no_mangle]
pub unsafe extern "C" fn header_map_insert(
    handle: *mut HeaderMap,
    key: *const c_char,
    value: *const c_char,
) -> bool {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return false;
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return false;
        }
    };

    let r_value = match to_rust_str(value, "parse value error") {
        Some(v) => v,
        None => {
            return false;
        }
    };

    let value: HeaderValue = match HeaderValue::from_str(r_value) {
        Ok(v) => v,
        Err(e) => {
            let err = format!("{r_value} convert to header failed. {e}");
            update_last_error(HttpErrorKind::Other, anyhow!(err));
            return false;
        }
    };

    let mut header_map = Box::from_raw(handle);
    let ret = match header_map.insert(r_key, value) {
        Some(_) => true,
        None => true,
    };

    Box::leak(header_map);

    ret
}

/// Inserts a key-value pair into the map.
///
/// If the map did not previously have this key present, then `false` is
/// returned.
///
/// If the map did have this key present, the new value is pushed to the end
/// of the list of values currently associated with the key. The key is not
/// updated, though; this matters for types that can be `==` without being
/// identical.
#[no_mangle]
pub unsafe extern "C" fn header_map_append(
    handle: *mut HeaderMap,
    key: *const c_char,
    value: *const c_char,
) -> bool {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return false;
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return false;
        }
    };
    let r_value = match to_rust_str(value, "parse value error") {
        Some(v) => v,
        None => {
            return false;
        }
    };

    let value: HeaderValue = match HeaderValue::from_str(r_value) {
        Ok(v) => v,
        Err(e) => {
            let err = format!("{r_value} convert to header failed. {e}");
            update_last_error(HttpErrorKind::Other, anyhow!(err));
            return false;
        }
    };

    let mut header_map = Box::from_raw(handle);
    header_map.append(r_key, value);

    Box::leak(header_map);

    true
}

/// Removes a key from the map, returning the value associated with the key.
///
/// Returns `None` if the map does not contain the key. If there are
/// multiple values associated with the key, then the first one is returned.
/// See `remove_entry_mult` on `OccupiedEntry` for an API that yields all
/// values.
#[no_mangle]
pub unsafe extern "C" fn header_map_remove(handle: *mut HeaderMap, key: *const c_char) -> bool {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return false;
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return false;
        }
    };

    let mut header_map = Box::from_raw(handle);
    header_map.remove(r_key);
    Box::leak(header_map);

    true
}

///Don't forget free
#[no_mangle]
pub unsafe extern "C" fn header_map_get(
    handle: *mut HeaderMap,
    key: *const c_char,
) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return ptr::null_mut();
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };

    let header_map = Box::from_raw(handle);

    let r_value: Option<&HeaderValue> = header_map.get(r_key);

    let ret = match r_value {
        Some(v) => match v.to_str() {
            Ok(v) => Box::into_raw(Box::new(RString::new(v.to_string()))),
            Err(e) => {
                update_last_error(HttpErrorKind::CharConversion, Error::new(e));
                ptr::null_mut()
            }
        },
        None => ptr::null_mut(),
    };

    Box::leak(header_map);

    ret
}

/// Returns the number of headers stored in the map.
///
/// This number represents the total number of **values** stored in the map.
/// This number can be greater than or equal to the number of **keys**
/// stored given that a single key may have more than one associated value.
#[no_mangle]
pub unsafe extern "C" fn header_map_len(handle: *mut HeaderMap) -> i32 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return -1;
    }

    let header_map = Box::from_raw(handle);
    let ret = header_map.len();
    Box::leak(header_map);

    ret as i32
}

/// Returns the number of keys stored in the map.
///
/// This number will be less than or equal to `len()` as each key may have
/// more than one associated value.
///
#[no_mangle]
pub unsafe extern "C" fn header_map_keys_len(handle: *mut HeaderMap) -> i32 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return -1;
    }

    let header_map = Box::from_raw(handle);
    let ret = header_map.keys_len();
    Box::leak(header_map);

    ret as i32
}

/// Clears the map, removing all key-value pairs. Keeps the allocated memory
/// for reuse.
#[no_mangle]
pub unsafe extern "C" fn header_map_clear(handle: *mut HeaderMap) {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
    } else {
        let mut header_map = Box::from_raw(handle);
        header_map.clear();
        Box::leak(header_map);
    }
}

/// Returns the number of headers the map can hold without reallocating.
///
/// This number is an approximation as certain usage patterns could cause
/// additional allocations before the returned capacity is filled.
#[no_mangle]
pub unsafe extern "C" fn header_map_capacity(handle: *mut HeaderMap) -> i32 {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return -1;
    }

    let header_map = Box::from_raw(handle);
    let ret = header_map.capacity();
    Box::leak(header_map);

    ret as i32
}

/// Reserves capacity for at least `additional` more headers to be inserted
/// into the `HeaderMap`.
///
/// The header map may reserve more space to avoid frequent reallocations.
/// Like with `with_capacity`, this will be a "best effort" to avoid
/// allocations until `additional` more headers are inserted. Certain usage
/// patterns could cause additional allocations before the number is
/// reached.
#[no_mangle]
pub unsafe extern "C" fn header_map_reserve(handle: *mut HeaderMap, additional: u32) {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
    } else {
        let mut header_map = Box::from_raw(handle);
        header_map.reserve(additional as usize);
        Box::leak(header_map);
    }
}

/// Returns a view of all values associated with a key.
///
/// The returned view does not incur any allocations and allows iterating
/// the values associated with the key.  See [`GetAll`] for more details.
/// Returns `None` if there are no values associated with the key.
#[no_mangle]
pub unsafe extern "C" fn header_map_get_all(
    handle: *mut HeaderMap,
    key: *const c_char,
) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return ptr::null_mut();
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };

    let mut value: String = String::default();

    let header_map = Box::from_raw(handle);
    let all_values = header_map.get_all(r_key);
    for v in all_values {
        match v.to_str() {
            Ok(v) => {
                value.push_str(v);
                value.push(';');
            }
            _ => {}
        }
    }
    if value.ends_with(';') {
        value.pop();
    }

    Box::leak(header_map);

    Box::into_raw(Box::new(RString::new(value.to_string())))
}

/// Returns true if the map contains a value for the specified key.
/// Return -1 if function failed.
/// why not use bool? because bk is bool.
/// If only return false,can't show function failed or isn't contains.
#[no_mangle]
pub unsafe extern "C" fn header_map_contains_key(
    handle: *mut HeaderMap,
    key: *const c_char,
) -> bool {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return false;
    }

    let r_key = match to_rust_str(key, "parse key error") {
        Some(v) => v,
        None => {
            return false;
        }
    };

    let header_map = Box::from_raw(handle);
    let ret = header_map.contains_key(r_key);
    Box::leak(header_map);

    ret
}

//TODO get keys array-string
#[no_mangle]
pub unsafe extern "C" fn header_map_keys(handle: *mut HeaderMap) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return ptr::null_mut();
    }

    let mut keys: String = String::default();

    let header_map = Box::from_raw(handle);
    let all_keys = header_map.keys();
    for v in all_keys {
        keys.push_str(v.as_str());
        keys.push(';');
    }
    if keys.ends_with(';') {
        keys.pop();
    }

    Box::leak(header_map);

    Box::into_raw(Box::new(RString::new(keys)))
}

//TODO get values array-string
#[no_mangle]
pub unsafe extern "C" fn header_map_values(handle: *mut HeaderMap) -> *mut RString {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("header_map handle is null"),
        );
        return ptr::null_mut();
    }

    let header_map = Box::from_raw(handle);
    let r_value: Vec<&str> = header_map
        .values()
        .into_iter()
        .map(|v| v.to_str().unwrap_or_else(|_| "opaque"))
        .collect();

    let ret = format!("{:?}", r_value);
    Box::leak(header_map);

    Box::into_raw(Box::new(RString::new(ret)))
}

#[no_mangle]
pub unsafe extern "C" fn header_map_destroy(handle: *mut HeaderMap) {
    if handle.is_null() {
        return;
    }
    drop(Box::from_raw(handle));
}
