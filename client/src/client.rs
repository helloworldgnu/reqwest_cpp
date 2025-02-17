use crate::ffi::*;
use anyhow::{anyhow, Error};
use ::{function, response};
use http_err::HttpErrorKind;
use libc::c_char;
use reqwest::blocking::{Client, ClientBuilder, Request, RequestBuilder};
use reqwest::header::HeaderMap;
use reqwest::{redirect, Method, Url};
use std::net::{IpAddr, SocketAddr};
use std::{ptr, slice};
use utils::extract_file_name;

/// Constructs a new `ClientBuilder`.
#[no_mangle]
pub unsafe extern "C" fn new_client_builder() -> *mut ClientBuilder {
    Box::into_raw(Box::new(ClientBuilder::new()))
}

/// Sets the default headers for every request.
#[no_mangle]
pub unsafe extern "C" fn client_builder_default_headers(
    handle: *mut ClientBuilder,
    header_map: *mut HeaderMap,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use default_headers"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let header_map = Box::from_raw(header_map);
    let result = r_client_builder.default_headers(*header_map);
    Box::into_raw(Box::new(result))
}

/// Sets the `User-Agent` header to be used by this client.
#[no_mangle]
pub unsafe extern "C" fn client_builder_user_agent(
    handle: *mut ClientBuilder,
    value: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("user agent handle is null when use user_agent"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(value, "") {
        None => {
            return ptr::null_mut();
        }
        Some(v) => v,
    };

    let r_client_builder: Box<ClientBuilder> = Box::from_raw(handle);
    let result: ClientBuilder = r_client_builder.user_agent(r_value);
    let res = Box::into_raw(Box::new(result));

    res
}

/// Set a `redirect::Policy` for this client.
///
/// Default will follow redirects up to a maximum of 10.
#[no_mangle]
pub unsafe extern "C" fn client_builder_redirect(
    handle: *mut ClientBuilder,
    policy: usize,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use redirect"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let r_policy: redirect::Policy = redirect::Policy::limited(policy);
    let result = r_client_builder.redirect(r_policy);
    Box::into_raw(Box::new(result))
}

/// Enable or disable automatic setting of the `Referer` header.
///
/// Default is `true`.
#[no_mangle]
pub unsafe extern "C" fn client_builder_referer(
    handle: *mut ClientBuilder,
    enable: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use referer"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.referer(enable);
    Box::into_raw(Box::new(result))
}

// Proxy options

/// Add a `Proxy` to the list of proxies the `Client` will use.
///
/// # Note
///
/// Adding a proxy will disable the automatic usage of the "system" proxy.
#[no_mangle]
pub unsafe extern "C" fn client_builder_proxy(
    handle: *mut ClientBuilder,
    proxy: *mut reqwest::Proxy,
) -> *mut ClientBuilder {
    if handle.is_null() || proxy.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle or proxy is null"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.proxy(*Box::from_raw(proxy));
    Box::into_raw(Box::new(result))
}

// Timeout options

/// Set a timeout for connect, read and write operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
#[no_mangle]
pub unsafe extern "C" fn client_builder_timeout(
    handle: *mut ClientBuilder,
    millisecond: *const u64,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use timeout"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.timeout(u64_to_millis_duration(millisecond));
    Box::into_raw(Box::new(result))
}

// Connect Timeout options

/// Set a timeout for connect operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
#[no_mangle]
pub unsafe extern "C" fn client_builder_connect_timeout(
    handle: *mut ClientBuilder,
    millisecond: *const u64,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use timeout"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.connect_timeout(u64_to_millis_duration(millisecond));
    Box::into_raw(Box::new(result))
}

// HTTP options

/// Set an optional timeout for idle sockets being kept-alive.
///
/// Pass `None` to disable timeout.
///
/// Default is 90 seconds.
#[no_mangle]
pub unsafe extern "C" fn client_builder_pool_idle_timeout(
    handle: *mut ClientBuilder,
    millisecond: *const u64,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use pool_idle_timeout"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.pool_idle_timeout(u64_to_millis_duration(millisecond));

    Box::into_raw(Box::new(result))
}

/// Sets the maximum idle connection per host allowed in the pool.
#[no_mangle]
pub unsafe extern "C" fn client_builder_pool_max_idle_per_host(
    handle: *mut ClientBuilder,
    max: usize,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use pool_max_idle_per_host"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.pool_max_idle_per_host(max);
    Box::into_raw(Box::new(result))
}

/// Sets the maximum idle connection per host allowed in the pool.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http1_title_case_headers(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use timeout"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http1_title_case_headers();
    Box::into_raw(Box::new(result))
}

/// Set whether HTTP/1 connections will accept obsolete line folding for
/// header values.
///
/// Newline codepoints (`\r` and `\n`) will be transformed to spaces when
/// parsing.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http1_allow_obsolete_multiline_headers_in_responses(
    handle: *mut ClientBuilder,
    val: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(HttpErrorKind::HttpHandleNull, anyhow!(
            "client_builder handle is null when use http1_allow_obsolete_multiline_headers_in_responses"
        ));
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http1_allow_obsolete_multiline_headers_in_responses(val);
    Box::into_raw(Box::new(result))
}

/// Only use HTTP/1.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http1_only(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http1_only"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http1_only();
    Box::into_raw(Box::new(result))
}

/// Allow HTTP/0.9 responses
#[no_mangle]
pub unsafe extern "C" fn client_builder_http09_responses(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http09_responses"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http09_responses();
    Box::into_raw(Box::new(result))
}

/// Only use HTTP/2.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http2_prior_knowledge(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_prior_knowledge"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http2_prior_knowledge();
    Box::into_raw(Box::new(result))
}

/// Sets the `SETTINGS_INITIAL_WINDOW_SIZE` option for HTTP2 stream-level flow control.
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http2_initial_stream_window_size(
    handle: *mut ClientBuilder,
    size: *mut u32,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_initial_stream_window_size"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http2_initial_stream_window_size(*size);
    Box::into_raw(Box::new(result))
}

/// Sets the max connection-level flow control for HTTP2
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http2_initial_connection_window_size(
    handle: *mut ClientBuilder,
    size: *mut u32,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_initial_connection_window_size"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http2_initial_connection_window_size(*size);
    Box::into_raw(Box::new(result))
}

/// Sets whether to use an adaptive flow control.
///
/// Enabling this will override the limits set in `http2_initial_stream_window_size` and
/// `http2_initial_connection_window_size`.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http2_adaptive_window(
    handle: *mut ClientBuilder,
    enable: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_adaptive_window"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http2_adaptive_window(enable);
    Box::into_raw(Box::new(result))
}

/// Sets the maximum frame size to use for HTTP2.
///
/// Default is currently 16,384 but may change internally to optimize for common uses.
#[no_mangle]
pub unsafe extern "C" fn client_builder_http2_max_frame_size(
    handle: *mut ClientBuilder,
    size: *mut u32,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_adaptive_window"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.http2_max_frame_size(*size);
    Box::into_raw(Box::new(result))
}

// TCP options

/// Set whether sockets have `TCP_NODELAY` enabled.
///
/// Default is `true`.
#[no_mangle]
pub unsafe extern "C" fn client_builder_tcp_nodelay(
    handle: *mut ClientBuilder,
    enable: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_adaptive_window"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.tcp_nodelay(enable);
    Box::into_raw(Box::new(result))
}

/// Bind to a local IP Address.
#[no_mangle]
pub unsafe extern "C" fn client_builder_local_address(
    handle: *mut ClientBuilder,
    local_address: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use local_address"),
        );
        return ptr::null_mut();
    }

    let r_local_address_str: &str;
    match to_rust_str(local_address, "arg is local_address") {
        Some(s) => r_local_address_str = s,
        None => {
            return ptr::null_mut();
        }
    }

    let r_local_address: IpAddr;
    match r_local_address_str.parse() {
        Ok(v) => r_local_address = v,
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e).context("ip illegality"));
            return ptr::null_mut();
        }
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.local_address(r_local_address);
    Box::into_raw(Box::new(result))
}

/// Set that all sockets have `SO_KEEPALIVE` set with the supplied duration.
///
/// If `None`, the option will not be set.
#[no_mangle]
pub unsafe extern "C" fn client_builder_tcp_keepalive(
    handle: *mut ClientBuilder,
    millisecond: *const u64,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use tcp_keepalive"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.tcp_keepalive(u64_to_millis_duration(millisecond));

    Box::into_raw(Box::new(result))
}

// TLS options

/// Add a custom root certificate.
///
/// This allows connecting to a server that has a self-signed
/// certificate for example. This **does not** replace the existing
/// trusted store.
#[no_mangle]
pub unsafe extern "C" fn client_builder_add_root_certificate(
    handle: *mut ClientBuilder,
    cert_path: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use add_root_certificate"),
        );
        return ptr::null_mut();
    }

    let r_cert_path: &str;
    match to_rust_str(cert_path, "cert_path parse failed") {
        Some(s) => r_cert_path = s,
        None => {
            return ptr::null_mut();
        }
    }

    let der = match std::fs::read(r_cert_path) {
        Ok(v) => v,
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e));
            return ptr::null_mut();
        }
    };

    let cert = match reqwest::Certificate::from_der(&der) {
        Ok(v) => v,
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e));
            return ptr::null_mut();
        }
    };

    let r_client_builder = Box::from_raw(handle);
    Box::into_raw(Box::new(r_client_builder.add_root_certificate(cert)))
}

/// Controls the use of built-in system certificates during certificate validation.
///
/// Defaults to `true` -- built-in system certs will be used.
///
/// # Optional
///
/// This requires the optional `default-tls`, `native-tls`, or `rustls-tls(-...)`
/// feature to be enabled.
#[no_mangle]
pub unsafe extern "C" fn client_builder_tls_built_in_root_certs(
    handle: *mut ClientBuilder,
    tls_built_in_root_certs: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use tls_built_in_root_certs"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.tls_built_in_root_certs(tls_built_in_root_certs);
    Box::into_raw(Box::new(result))
}

/// Controls the use of certificate validation.
///
/// Defaults to `false`.
///
/// # Warning
///
/// You should think very carefully before using this method. If
/// invalid certificates are trusted, *any* certificate for *any* site
/// will be trusted for use. This includes expired certificates. This
/// introduces significant vulnerabilities, and should only be used
/// as a last resort.
#[no_mangle]
pub unsafe extern "C" fn client_builder_danger_accept_invalid_certs(
    handle: *mut ClientBuilder,
    accept_invalid_certs: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use danger_accept_invalid_certs"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.danger_accept_invalid_certs(accept_invalid_certs);
    Box::into_raw(Box::new(result))
}

/// Controls the use of TLS server name indication.
///
/// Defaults to `true`.
#[no_mangle]
pub unsafe extern "C" fn client_builder_tls_sni(
    handle: *mut ClientBuilder,
    tls_sni: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use danger_accept_invalid_certs"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.tls_sni(tls_sni);
    Box::into_raw(Box::new(result))
}

/// Set the minimum required TLS version for connections.
///
/// By default the TLS backend's own default is used.
///
/// # Errors
///
/// A value of `tls::Version::TLS_1_3` will cause an error with the
/// `native-tls`/`default-tls` backend. This does not mean the version
/// isn't supported, just that it can't be set as a minimum due to
/// technical limitations.
///
/// # Optional
///
/// This requires the optional `default-tls`, `native-tls`, or `rustls-tls(-...)`
/// feature to be enabled.
#[no_mangle]
pub unsafe extern "C" fn client_builder_min_tls_version(
    handle: *mut ClientBuilder,
    version: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() || version.is_null() {
        update_last_error(
            HttpErrorKind::Other,
            anyhow!("client_builder handle or version is null when use min_tls_version"),
        );
        return ptr::null_mut();
    }

    let r_version = match to_rust_str(version, "version to str failed") {
        Some("1.0") => reqwest::tls::Version::TLS_1_0,
        Some("1.1") => reqwest::tls::Version::TLS_1_1,
        Some("1.2") => reqwest::tls::Version::TLS_1_2,
        Some("1.3") => reqwest::tls::Version::TLS_1_3,
        _ => {
            return ptr::null_mut();
        }
    };

    let result: ClientBuilder = Box::from_raw(handle).min_tls_version(r_version);
    Box::into_raw(Box::new(result))
}

/// Set the maximum allowed TLS version for connections.
///
/// By default there's no maximum.
///
/// # Errors
///
/// A value of `tls::Version::TLS_1_3` will cause an error with the
/// `native-tls`/`default-tls` backend. This does not mean the version
/// isn't supported, just that it can't be set as a maximum due to
/// technical limitations.
///
/// # Optional
///
/// This requires the optional `default-tls`, `native-tls`, or `rustls-tls(-...)`
/// feature to be enabled.
#[no_mangle]
pub unsafe extern "C" fn client_builder_max_tls_version(
    handle: *mut ClientBuilder,
    version: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() || version.is_null() {
        update_last_error(
            HttpErrorKind::Other,
            anyhow!("client_builder handle or version is null when use max_tls_version"),
        );
        return ptr::null_mut();
    }

    let r_version = match to_rust_str(version, "version to str failed") {
        Some("1.0") => reqwest::tls::Version::TLS_1_0,
        Some("1.1") => reqwest::tls::Version::TLS_1_1,
        Some("1.2") => reqwest::tls::Version::TLS_1_2,
        Some("1.3") => reqwest::tls::Version::TLS_1_3,
        _ => {
            return ptr::null_mut();
        }
    };

    let result: ClientBuilder = Box::from_raw(handle).max_tls_version(r_version);
    Box::into_raw(Box::new(result))
}

/// Disables the trust-dns async resolver.
///
/// This method exists even if the optional `trust-dns` feature is not enabled.
/// This can be used to ensure a `Client` doesn't use the trust-dns async resolver
/// even if another dependency were to enable the optional `trust-dns` feature.
#[no_mangle]
pub unsafe extern "C" fn client_builder_no_trust_dns(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use no_trust_dns"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.no_hickory_dns();
    Box::into_raw(Box::new(result))
}

#[no_mangle]
pub unsafe extern "C" fn client_builder_no_hickory_dns(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use no_hickory_dns"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.no_hickory_dns();
    Box::into_raw(Box::new(result))
}

#[no_mangle]
pub unsafe extern "C" fn client_builder_hickory_dns(
    handle: *mut ClientBuilder,
    enable: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use hickory_dns"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.hickory_dns(enable);
    Box::into_raw(Box::new(result))
}

/// Restrict the Client to be used with HTTPS only requests.
///
/// Defaults to false.
#[no_mangle]
pub unsafe extern "C" fn client_builder_https_only(
    handle: *mut ClientBuilder,
    enable: bool,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null when use http2_prior_knowledge"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.https_only(enable);
    Box::into_raw(Box::new(result))
}

/// Override DNS resolution for specific domains to a particular IP address.
///
/// Warning
///
/// Since the DNS protocol has no notion of ports, if you wish to send
/// traffic to a particular port you must include this port in the URL
/// itself, any port in the overridden addr will be ignored and traffic sent
/// to the conventional port for the given scheme (e.g. 80 for http).
#[no_mangle]
pub unsafe extern "C" fn client_builder_resolve(
    handle: *mut ClientBuilder,
    domain: *const c_char,
    socket_addr: *const c_char,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client builder handle is null when use resolve"),
        );
        return ptr::null_mut();
    }

    let r_domain = match to_rust_str(domain, "domain to str failed") {
        Some(v) => v,
        _ => {
            return ptr::null_mut();
        }
    };
    let r_socket_addr_str = match to_rust_str(socket_addr, "socket_addr to str failed") {
        Some(v) => v,
        _ => {
            return ptr::null_mut();
        }
    };
    let r_socket_addr: SocketAddr = match r_socket_addr_str.parse() {
        Ok(v) => v,
        _ => {
            update_last_error(
                HttpErrorKind::Other,
                anyhow!("{} parse fail", r_socket_addr_str),
            );
            return ptr::null_mut();
        }
    };

    let result = Box::from_raw(handle).resolve(r_domain, r_socket_addr);
    Box::into_raw(Box::new(result))
}

/// Override DNS resolution for specific domains to particular IP addresses.
///
/// Warning
///
/// Since the DNS protocol has no notion of ports, if you wish to send
/// traffic to a particular port you must include this port in the URL
/// itself, any port in the overridden addresses will be ignored and traffic sent
/// to the conventional port for the given scheme (e.g. 80 for http).
#[no_mangle]
pub unsafe extern "C" fn client_builder_resolve_to_addrs(
    handle: *mut ClientBuilder,
    domain: *const c_char,
    socket_addr_array: *const *const c_char,
    len: usize,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client builder handle is null when use resolve"),
        );
        return ptr::null_mut();
    }

    let r_domain = match to_rust_str(domain, "domain to str failed") {
        Some(v) => v,
        _ => {
            return ptr::null_mut();
        }
    };

    let r_socket_addr_array: &[*const c_char] = slice::from_raw_parts(socket_addr_array, len);
    let mut r_socket_addrs: Vec<SocketAddr> = Vec::new();
    for socket_addr in r_socket_addr_array {
        let r_socket_addr_str =
            match to_rust_str(socket_addr.to_owned(), "socket_addr to str failed") {
                Some(v) => v,
                _ => {
                    return ptr::null_mut();
                }
            };
        let r_socket_addr: SocketAddr = match r_socket_addr_str.parse() {
            Ok(v) => v,
            _ => {
                update_last_error(
                    HttpErrorKind::Other,
                    anyhow!("{} parse fail", r_socket_addr_str),
                );
                return ptr::null_mut();
            }
        };
        r_socket_addrs.push(r_socket_addr)
    }

    let result = Box::from_raw(handle).resolve_to_addrs(r_domain, &r_socket_addrs);
    Box::into_raw(Box::new(result))
}

///Generally not required
#[no_mangle]
pub unsafe extern "C" fn client_builder_destroy(handle: *mut ClientBuilder) {
    if !handle.is_null() {
        drop(Box::from_raw(handle));
    }
}

/// Returns a `Client` that uses this `ClientBuilder` configuration.
///
/// # Errors
///
/// This method fails if TLS backend cannot be initialized, or the resolver
/// cannot load the system configuration.
#[no_mangle]
pub unsafe extern "C" fn client_builder_build_client(handle: *mut ClientBuilder) -> *mut Client {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client builder handle is null when use client_builder_build_client"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    match r_client_builder.build() {
        Ok(c) => Box::into_raw(Box::new(c)),
        Err(e) => {
            let err = Error::new(e).context("Unable to build client");
            update_last_error(HttpErrorKind::Other, err);
            ptr::null_mut()
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn client_builder_use_rustls(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.use_rustls_tls();
    Box::into_raw(Box::new(result))
}

#[no_mangle]
pub unsafe extern "C" fn client_builder_use_native_tls(
    handle: *mut ClientBuilder,
) -> *mut ClientBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client_builder handle is null"),
        );
        return ptr::null_mut();
    }

    let r_client_builder = Box::from_raw(handle);
    let result = r_client_builder.use_native_tls();
    Box::into_raw(Box::new(result))
}

//It is usually necessary to use
#[no_mangle]
pub unsafe extern "C" fn client_destroy(handle: *mut Client) {
    if !handle.is_null() {
        drop(Box::from_raw(handle));
    }
}

/// Convenience method to make a `GET` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_get(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use get"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };
    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).get(r_value);
    Box::into_raw(Box::new(rb))
}

/// Convenience method to make a `POST` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_post(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use post"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };
    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).post(r_value);
    Box::into_raw(Box::new(rb))
}

/// Convenience method to make a `PUT` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_put(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use put"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };
    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).put(r_value);
    Box::into_raw(Box::new(rb))
}

/// Convenience method to make a `PATCH` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_patch(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use put"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };
    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).patch(r_value);
    Box::into_raw(Box::new(rb))
}

/// Convenience method to make a `DELETE` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_delete(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use put"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };
    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).delete(r_value);
    Box::into_raw(Box::new(rb))
}

/// Convenience method to make a `HEAD` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_head(
    handle: *mut Client,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use put"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };

    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let rb = (*handle).head(r_value);
    Box::into_raw(Box::new(rb))
}

/// Start building a `Request` with the `Method` and `Url`.
///
/// Returns a `RequestBuilder`, which will allow setting headers and
/// request body before sending.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
#[no_mangle]
pub unsafe extern "C" fn client_request(
    handle: *mut Client,
    method: *const c_char,
    url: *const c_char,
) -> *mut RequestBuilder {
    if handle.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle is null when use request"),
        );
        return ptr::null_mut();
    }

    let r_value = match to_rust_str(url, "url parse string error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };

    if Url::parse(r_value).is_err() {
        update_last_error(HttpErrorKind::Other, anyhow!("url illegality"));
        return ptr::null_mut();
    }

    let method_str = match to_rust_str(method, "method parse string error") {
        Some(v) => v,
        None => {
            return ptr::null_mut();
        }
    };

    let r_method = match Method::from_bytes(method_str.as_bytes()) {
        Ok(v) => v,
        Err(e) => {
            update_last_error(HttpErrorKind::Other, Error::new(e));
            return ptr::null_mut();
        }
    };

    let rb = (*handle).request(r_method, r_value);
    Box::into_raw(Box::new(rb))
}

/// Executes a `Request`.
///
/// A `Request` can be built manually with `Request::new()` or obtained
/// from a RequestBuilder with `RequestBuilder::build()`.
///
/// You should prefer to use the `RequestBuilder` and
/// `RequestBuilder::send()`.
///
/// # Errors
///
/// This method fails if there was an error while sending request,
/// or redirect limit was exhausted.
#[no_mangle]
pub unsafe extern "C" fn client_execute(
    handle: *mut Client,
    request: *mut Request,
) -> *mut response::Response {
    if handle.is_null() || request.is_null() {
        update_last_error(
            HttpErrorKind::HttpHandleNull,
            anyhow!("client handle or request is null when use request"),
        );
        return ptr::null_mut();
    }

    let client = unsafe { Box::from_raw(handle) };

    let req = Box::from_raw(request);
    let url = req.url().to_string();
    let result = client.execute(*req);
    let resp = match result {
        Ok(v) => {
            let resp = response::Response::new(Some(v));
            Box::into_raw(Box::new(resp))
        },
        Err(err) => {
            update_last_error(
                HttpErrorKind::Other,
                anyhow!(
                    "{}#{}:{}, {:?}, {}",
                    extract_file_name(file!()),
                    function!(),
                    line!(),
                    err,
                    url
                ),
            );
            ptr::null_mut()
        }
    };

    Box::leak(client);

    resp
}

#[cfg(test)]
mod tests {
    use super::*;

    /// url error back when send
    #[test]
    fn into_url_scheme() {
        let c = reqwest::blocking::Client::new();
        let req = c.request(Method::GET, "url");
        let _ = match req.send() {
            Ok(_) => (),
            Err(e) => {
                print!("{:#?}", e)
            }
        };
    }

    #[test]
    fn test_addr() {
        let cb = reqwest::blocking::ClientBuilder::new();
        println!("addr of client_builder new={:p}", &cb);
        let cb2 = cb.user_agent("AB");
        println!("addr of client_builder user_agent={:p}", &cb2);
    }
}
