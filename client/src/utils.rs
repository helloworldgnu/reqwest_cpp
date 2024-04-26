use chrono::Local;
use fern;
use log::LevelFilter;
use reqwest;
use std::{error::Error, sync::Once};

/// Initialize the global logger and log to `rest_client.log`.
///
/// Note that this is an idempotent function, so you can call it as many
/// times as you want and logging will only be initialized the first time.
#[no_mangle]
pub extern "C" fn initialize_logging() {
    static INITIALIZE: Once = Once::new();
    INITIALIZE.call_once(|| {
        fern::Dispatch::new()
            .format(|out, message, record| {
                out.finish(format_args!(
                    "{} {:7} ({:?}#{:?}): {}{}",
                    Local::now().format("[%Y-%m-%d][%H:%M:%S]"),
                    record.level(),
                    record.module_path(),
                    record.line(),
                    message,
                    if cfg!(windows) { "\r" } else { "" }
                ))
            })
            .level(LevelFilter::Debug)
            .chain(fern::log_file("rest_client.log").unwrap())
            .apply()
            .unwrap();
    });
}

// Log an error and each successive thing which caused it.
//pub fn backtrace(e : &Error) {
//    error!("Error: {}", e);
//
//    for cause in e.iter().skip(1) {
//        warn!("\tCaused By: {}",cause );
//    }
//}

pub(crate) unsafe fn parse_err(err: &reqwest::Error, kind: *mut u32, value: *mut i32) {
    /*
    pub(crate) enum Kind {
        None,
        Unknown,
        TimeOut,
        Builder,
        Request,
        Redirect,
        Status(StatusCode),
        Body,
        Decode,
        Upgrade,
    }
    */

    if err.is_timeout() {
        *kind = 1002;
    } else if err.is_status() {
        *kind = 1006;
        let v = match err.status() {
            Some(code) => code.as_u16() as i32,
            None => 0,
        };

        if !value.is_null() {
            *value = v;
        }
    } else if err.is_builder() {
        *kind = 1003;
    } else if err.is_request() {
        *kind = 1004;
    } else if err.is_redirect() {
        *kind = 1005;
    } else if err.is_body() {
        *kind = 1007;
    } else if err.is_decode() {
        *kind = 1008;
    } else {
        *kind = 1001;
    }
}

pub(crate) unsafe fn parse_read_err(err: &std::io::Error, kind: *mut u32) {
    use std::io::ErrorKind;

    let mut kk = 0u32;

    match err.kind() {
        ErrorKind::TimedOut => {
            kk = 2002;
        }
        ErrorKind::ConnectionRefused => {
            kk = 2003;
        }
        ErrorKind::ConnectionReset => {
            kk = 2004;
        }
        ErrorKind::ConnectionAborted => {
            kk = 2005;
        }
        ErrorKind::Interrupted => {
            kk = 2006;
        }
        ErrorKind::UnexpectedEof => {
            kk = 2007;
        }
        _ => {}
    }

    if kk != 0u32 {
        *kind = kk;
        return;
    }

    let mut source = err.source();
    while let Some(e) = source {
        kk = 0u32;
        if let Some(io) = e.downcast_ref::<reqwest::Error>() {
            parse_err(&io, &mut kk as *mut u32, std::ptr::null_mut());
            if kk != 0u32 {
                *kind = kk;
                return;
            }
        }

        if let Some(io) = e.downcast_ref::<std::io::Error>() {
            match io.kind() {
                ErrorKind::TimedOut => {
                    kk = 2002;
                }
                ErrorKind::ConnectionRefused => {
                    kk = 2003;
                }
                ErrorKind::ConnectionReset => {
                    kk = 2004;
                }
                ErrorKind::ConnectionAborted => {
                    kk = 2005;
                }
                ErrorKind::Interrupted => {
                    kk = 2006;
                }
                ErrorKind::UnexpectedEof => {
                    kk = 2007;
                }
                _ => {}
            }

            if kk != 0u32 {
                *kind = kk;
                return;
            }
        }

        source = e.source();
    }
}
