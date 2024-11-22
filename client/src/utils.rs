use chrono::Local;
use fern;
use http_err::HttpErrorKind;
use log::LevelFilter;
use reqwest;
use std::io::ErrorKind;
use std::{error::Error, sync::Once};
use std::path::Path;

pub fn extract_file_name<P: AsRef<Path>>(p: P) -> String {
    let path = p.as_ref();
    match path.file_stem() {
        Some(s) => s.to_str().unwrap_or("").into(),
        None => {
            path.to_str().unwrap_or("").into()
        }
    }
}

#[macro_export] macro_rules! function {
    () => {{
        fn f() {}
        fn type_name_of<T>(_: T) -> &'static str {
            std::any::type_name::<T>()
        }
        type_name_of(f)
            .rsplit("::")
            .find(|&part| part != "f" && part != "{{closure}}").unwrap_or("")
    }};
}

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

pub(crate) unsafe fn parse_err(err: &reqwest::Error, kind: &mut HttpErrorKind) {
    if err.is_timeout() {
        *kind = HttpErrorKind::HttpTimeout;
    } else if err.is_status() {
        *kind = HttpErrorKind::HttpStatus;
    } else if err.is_builder() {
        *kind = HttpErrorKind::HttpBuilder;
    } else if err.is_request() {
        *kind = HttpErrorKind::HttpRequest;
    } else if err.is_redirect() {
        *kind = HttpErrorKind::HttpRedirect;
    } else if err.is_body() {
        *kind = HttpErrorKind::HttpBody;
    } else if err.is_decode() {
        *kind = HttpErrorKind::HttpDecode;
    } else {
        *kind = HttpErrorKind::Other;

        let mut source = err.source();
        while let Some(e) = source {
            if let Some(io) = e.downcast_ref::<std::io::Error>() {
                parse_io_err(io, kind);
            }

            source = e.source();
        }
    }
}

pub(crate) unsafe fn parse_io_err(err: &std::io::Error, kind: &mut HttpErrorKind) {
    match err.kind() {
        ErrorKind::NotFound => {
            *kind = HttpErrorKind::NotFound;
        }
        ErrorKind::PermissionDenied => {
            *kind = HttpErrorKind::PermissionDenied;
        }
        ErrorKind::ConnectionRefused => {
            *kind = HttpErrorKind::ConnectionRefused;
        }
        ErrorKind::ConnectionReset => {
            *kind = HttpErrorKind::ConnectionReset;
        }
        /*
        ErrorKind::HostUnreachable => { *kind = HttpErrorKind::HostUnreachable; }
        ErrorKind::NetworkUnreachable => { *kind = HttpErrorKind::NetworkUnreachable; }
        */
        ErrorKind::ConnectionAborted => {
            *kind = HttpErrorKind::ConnectionAborted;
        }
        ErrorKind::NotConnected => {
            *kind = HttpErrorKind::NotConnected;
        }
        ErrorKind::AddrInUse => {
            *kind = HttpErrorKind::AddrInUse;
        }
        ErrorKind::AddrNotAvailable => {
            *kind = HttpErrorKind::AddrNotAvailable;
        }
        /*
        ErrorKind::NetworkDown => { *kind = HttpErrorKind::NetworkDown; }
        */
        ErrorKind::BrokenPipe => {
            *kind = HttpErrorKind::BrokenPipe;
        }
        ErrorKind::AlreadyExists => {
            *kind = HttpErrorKind::AlreadyExists;
        }
        ErrorKind::WouldBlock => {
            *kind = HttpErrorKind::WouldBlock;
        }
        /*
        ErrorKind::NotADirectory => { *kind = HttpErrorKind::NotADirectory; }
        ErrorKind::IsADirectory => { *kind = HttpErrorKind::IsADirectory; }
        ErrorKind::DirectoryNotEmpty => { *kind = HttpErrorKind::DirectoryNotEmpty; }
        ErrorKind::ReadOnlyFilesystem => { *kind = HttpErrorKind::ReadOnlyFilesystem; }
        ErrorKind::FilesystemLoop => { *kind = HttpErrorKind::FilesystemLoop; }
        ErrorKind::StaleNetworkFileHandle => { *kind = HttpErrorKind::StaleNetworkFileHandle; }
        */
        ErrorKind::InvalidInput => {
            *kind = HttpErrorKind::InvalidInput;
        }
        ErrorKind::InvalidData => {
            *kind = HttpErrorKind::InvalidData;
        }
        ErrorKind::TimedOut => {
            *kind = HttpErrorKind::TimedOut;
        }
        ErrorKind::WriteZero => {
            *kind = HttpErrorKind::WriteZero;
        }
        /*
        ErrorKind::StorageFull => { *kind = HttpErrorKind::StorageFull; }
        ErrorKind::NotSeekable => { *kind = HttpErrorKind::NotSeekable; }
        ErrorKind::FilesystemQuotaExceeded => { *kind = HttpErrorKind::FilesystemQuotaExceeded; }
        ErrorKind::FileTooLarge => { *kind = HttpErrorKind::FileTooLarge; }
        ErrorKind::ResourceBusy => { *kind = HttpErrorKind::ResourceBusy; }
        ErrorKind::ExecutableFileBusy => { *kind = HttpErrorKind::ExecutableFileBusy; }
        ErrorKind::Deadlock => { *kind = HttpErrorKind::Deadlock; }
        ErrorKind::CrossesDevices => { *kind = HttpErrorKind::CrossesDevices; }
        ErrorKind::TooManyLinks => { *kind = HttpErrorKind::TooManyLinks; }
        ErrorKind::InvalidFilename => { *kind = HttpErrorKind::InvalidFilename; }
        ErrorKind::ArgumentListTooLong => { *kind = HttpErrorKind::ArgumentListTooLong; }
         */
        ErrorKind::Interrupted => {
            *kind = HttpErrorKind::Interrupted;
        }
        ErrorKind::Unsupported => {
            *kind = HttpErrorKind::Unsupported;
        }
        ErrorKind::UnexpectedEof => {
            *kind = HttpErrorKind::UnexpectedEof;
        }
        ErrorKind::OutOfMemory => {
            *kind = HttpErrorKind::OutOfMemory;
        }
        ErrorKind::Other => {
            *kind = HttpErrorKind::Other;
        }
        /*
        ErrorKind::Uncategorized => { *kind = HttpErrorKind::Uncategorized; }
         */
        _ => {
            *kind = HttpErrorKind::Other;
        }
    }
}
