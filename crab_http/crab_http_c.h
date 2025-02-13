#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

namespace crab::http {

enum class HttpErrorKind {
  NoError,
  /// An entity was not found, often a file.
  NotFound,
  /// The operation lacked the necessary privileges to complete.
  PermissionDenied,
  /// The connection was refused by the remote server.
  ConnectionRefused,
  /// The connection was reset by the remote server.
  ConnectionReset,
  /// The remote host is not reachable.
  HostUnreachable,
  /// The network containing the remote host is not reachable.
  NetworkUnreachable,
  /// The connection was aborted (terminated) by the remote server.
  ConnectionAborted,
  /// The network operation failed because it was not connected yet.
  NotConnected,
  /// A socket address could not be bound because the address is already in
  /// use elsewhere.
  AddrInUse,
  /// A nonexistent interface was requested or the requested address was not
  /// local.
  AddrNotAvailable,
  /// The system's networking is down.
  NetworkDown,
  /// The operation failed because a pipe was closed.
  BrokenPipe,
  /// An entity already exists, often a file.
  AlreadyExists,
  /// The operation needs to block to complete, but the blocking operation was
  /// requested to not occur.
  WouldBlock,
  /// A filesystem object is, unexpectedly, not a directory.
  ///
  /// For example, a filesystem path was specified where one of the intermediate directory
  /// components was, in fact, a plain file.
  NotADirectory,
  /// The filesystem object is, unexpectedly, a directory.
  ///
  /// A directory was specified when a non-directory was expected.
  IsADirectory,
  /// A non-empty directory was specified where an empty directory was expected.
  DirectoryNotEmpty,
  /// The filesystem or storage medium is read-only, but a write operation was attempted.
  ReadOnlyFilesystem,
  /// Loop in the filesystem or IO subsystem; often, too many levels of symbolic links.
  ///
  /// There was a loop (or excessively long chain) resolving a filesystem object
  /// or file IO object.
  ///
  /// On Unix this is usually the result of a symbolic link loop; or, of exceeding the
  /// system-specific limit on the depth of symlink traversal.
  FilesystemLoop,
  /// Stale network file handle.
  ///
  /// With some network filesystems, notably NFS, an open file (or directory) can be invalidated
  /// by problems with the network or server.
  StaleNetworkFileHandle,
  /// A parameter was incorrect.
  InvalidInput,
  /// Data not valid for the operation were encountered.
  ///
  /// Unlike [`InvalidInput`], this typically means that the operation
  /// parameters were valid, however the error was caused by malformed
  /// input data.
  ///
  /// For example, a function that reads a file into a string will error with
  /// `InvalidData` if the file's contents are not valid UTF-8.
  ///
  /// [`InvalidInput`]: std::io::ErrorKind::InvalidInput
  InvalidData,
  /// The I/O operation's timeout expired, causing it to be canceled.
  TimedOut,
  /// An error returned when an operation could not be completed because a
  /// call to [`write`] returned [`Ok(0)`].
  ///
  /// This typically means that an operation could only succeed if it wrote a
  /// particular number of bytes but only a smaller number of bytes could be
  /// written.
  ///
  /// [`write`]: crate::io::Write::write
  /// [`Ok(0)`]: Ok
  WriteZero,
  /// The underlying storage (typically, a filesystem) is full.
  ///
  /// This does not include out of quota errors.
  StorageFull,
  /// Seek on unseekable file.
  ///
  /// Seeking was attempted on an open file handle which is not suitable for seeking - for
  /// example, on Unix, a named pipe opened with `File::open`.
  NotSeekable,
  /// Filesystem quota was exceeded.
  FilesystemQuotaExceeded,
  /// File larger than allowed or supported.
  ///
  /// This might arise from a hard limit of the underlying filesystem or file access API, or from
  /// an administratively imposed resource limitation.  Simple disk full, and out of quota, have
  /// their own errors.
  FileTooLarge,
  /// Resource is busy.
  ResourceBusy,
  /// Executable file is busy.
  ///
  /// An attempt was made to write to a file which is also in use as a running program.  (Not all
  /// operating systems detect this situation.)
  ExecutableFileBusy,
  /// Deadlock (avoided).
  ///
  /// A file locking operation would result in deadlock.  This situation is typically detected, if
  /// at all, on a best-effort basis.
  Deadlock,
  /// Cross-device or cross-filesystem (hard) link or rename.
  CrossesDevices,
  /// Too many (hard) links to the same filesystem object.
  ///
  /// The filesystem does not support making so many hardlinks to the same file.
  TooManyLinks,
  /// A filename was invalid.
  ///
  /// This error can also cause if it exceeded the filename length limit.
  InvalidFilename,
  /// Program argument list too long.
  ///
  /// When trying to run an external program, a system or process limit on the size of the
  /// arguments would have been exceeded.
  ArgumentListTooLong,
  /// This operation was interrupted.
  ///
  /// Interrupted operations can typically be retried.
  Interrupted,
  /// This operation is unsupported on this platform.
  ///
  /// This means that the operation can never succeed.
  Unsupported,
  /// An error returned when an operation could not be completed because an
  /// "end of file" was reached prematurely.
  ///
  /// This typically means that an operation could only succeed if it read a
  /// particular number of bytes but only a smaller number of bytes could be
  /// read.
  UnexpectedEof,
  /// An operation could not be completed, because it failed
  /// to allocate enough memory.
  OutOfMemory,
  /// A custom error that does not fall under any other I/O error kind.
  ///
  /// This can be used to construct your own [`Error`]s that do not match any
  /// [`std::io::ErrorKind`].
  ///
  /// This [`std::io::ErrorKind`] is not used by the standard library.
  ///
  /// Errors from the standard library that do not fall under any of the I/O
  /// error kinds cannot be `match`ed on, and will only match a wildcard (`_`) pattern.
  /// New [`std::io::ErrorKind`]s might be added in the future for some of those.
  Other,
  /// Any I/O error from the standard library that's not part of this list.
  ///
  /// Errors that are `Uncategorized` now may move to a different or a new
  /// [`std::io::ErrorKind`] variant in the future. It is not recommended to match
  /// an error against `Uncategorized`; use a wildcard match (`_`) instead.
  Uncategorized,
  HttpHandleNull,
  CharConversion,
  HttpTimeout,
  HttpBuilder,
  HttpRequest,
  HttpRedirect,
  HttpStatus,
  HttpBody,
  HttpDecode,
  HttpUpgrade,
};

struct Pair {
  const char *key;
  const char *value;
};

extern "C" {

/// Add a custom root certificate.
///
/// This allows connecting to a server that has a self-signed
/// certificate for example. This **does not** replace the existing
/// trusted store.
void *client_builder_add_root_certificate(void *handle, const char *cert_path);

/// Returns a `Client` that uses this `ClientBuilder` configuration.
///
/// # Errors
///
/// This method fails if TLS backend cannot be initialized, or the resolver
/// cannot load the system configuration.
void *client_builder_build_client(void *handle);

/// Set a timeout for connect operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
void *client_builder_connect_timeout(void *handle, const uint64_t *millisecond);

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
void *client_builder_danger_accept_invalid_certs(void *handle,
                                                          bool accept_invalid_certs);

/// Sets the default headers for every request.
void *client_builder_default_headers(void *handle, void *header_map);

///Generally not required
void client_builder_destroy(void *handle);

/// Allow HTTP/0.9 responses
void *client_builder_http09_responses(void *handle);

/// Set whether HTTP/1 connections will accept obsolete line folding for
/// header values.
///
/// Newline codepoints (`\r` and `\n`) will be transformed to spaces when
/// parsing.
void *client_builder_http1_allow_obsolete_multiline_headers_in_responses(void *handle,
                                                                                  bool val);

/// Only use HTTP/1.
void *client_builder_http1_only(void *handle);

/// Sets the maximum idle connection per host allowed in the pool.
void *client_builder_http1_title_case_headers(void *handle);

/// Sets whether to use an adaptive flow control.
///
/// Enabling this will override the limits set in `http2_initial_stream_window_size` and
/// `http2_initial_connection_window_size`.
void *client_builder_http2_adaptive_window(void *handle, bool enable);

/// Sets the max connection-level flow control for HTTP2
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
void *client_builder_http2_initial_connection_window_size(void *handle,
                                                                   uint32_t *size);

/// Sets the `SETTINGS_INITIAL_WINDOW_SIZE` option for HTTP2 stream-level flow control.
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
void *client_builder_http2_initial_stream_window_size(void *handle,
                                                               uint32_t *size);

/// Sets the maximum frame size to use for HTTP2.
///
/// Default is currently 16,384 but may change internally to optimize for common uses.
void *client_builder_http2_max_frame_size(void *handle, uint32_t *size);

/// Only use HTTP/2.
void *client_builder_http2_prior_knowledge(void *handle);

/// Restrict the Client to be used with HTTPS only requests.
///
/// Defaults to false.
void *client_builder_https_only(void *handle, bool enable);

/// Bind to a local IP Address.
void *client_builder_local_address(void *handle, const char *local_address);

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
void *client_builder_max_tls_version(void *handle, const char *version);

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
void *client_builder_min_tls_version(void *handle, const char *version);

void *client_builder_no_hickory_dns(void *handle);

void *client_builder_hickory_dns(void *handle, bool enable);

void *client_builder_use_rustls(void *handle);

void *client_builder_use_native_tls(void *handle);

/// Disables the trust-dns async resolver.
///
/// This method exists even if the optional `trust-dns` feature is not enabled.
/// This can be used to ensure a `Client` doesn't use the trust-dns async resolver
/// even if another dependency were to enable the optional `trust-dns` feature.
void *client_builder_no_trust_dns(void *handle);

/// Set an optional timeout for idle sockets being kept-alive.
///
/// Pass `None` to disable timeout.
///
/// Default is 90 seconds.
void *client_builder_pool_idle_timeout(void *handle, const uint64_t *millisecond);

/// Sets the maximum idle connection per host allowed in the pool.
void *client_builder_pool_max_idle_per_host(void *handle, uintptr_t max);

/// Add a `Proxy` to the list of proxies the `Client` will use.
///
/// # Note
///
/// Adding a proxy will disable the automatic usage of the "system" proxy.
void *client_builder_proxy(void *handle, void *proxy);

/// Set a `redirect::Policy` for this client.
///
/// Default will follow redirects up to a maximum of 10.
void *client_builder_redirect(void *handle, uintptr_t policy);

/// Enable or disable automatic setting of the `Referer` header.
///
/// Default is `true`.
void *client_builder_referer(void *handle, bool enable);

/// Override DNS resolution for specific domains to a particular IP address.
///
/// Warning
///
/// Since the DNS protocol has no notion of ports, if you wish to send
/// traffic to a particular port you must include this port in the URL
/// itself, any port in the overridden addr will be ignored and traffic sent
/// to the conventional port for the given scheme (e.g. 80 for http).
void *client_builder_resolve(void *handle,
                                      const char *domain,
                                      const char *socket_addr);

/// Override DNS resolution for specific domains to particular IP addresses.
///
/// Warning
///
/// Since the DNS protocol has no notion of ports, if you wish to send
/// traffic to a particular port you must include this port in the URL
/// itself, any port in the overridden addresses will be ignored and traffic sent
/// to the conventional port for the given scheme (e.g. 80 for http).
void *client_builder_resolve_to_addrs(void *handle,
                                               const char *domain,
                                               const char *const *socket_addr_array,
                                               uintptr_t len);

/// Set that all sockets have `SO_KEEPALIVE` set with the supplied duration.
///
/// If `None`, the option will not be set.
void *client_builder_tcp_keepalive(void *handle, const uint64_t *millisecond);

/// Set whether sockets have `TCP_NODELAY` enabled.
///
/// Default is `true`.
void *client_builder_tcp_nodelay(void *handle, bool enable);

/// Set a timeout for connect, read and write operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
void *client_builder_timeout(void *handle, const uint64_t *millisecond);

/// Controls the use of built-in system certificates during certificate validation.
///
/// Defaults to `true` -- built-in system certs will be used.
///
/// # Optional
///
/// This requires the optional `default-tls`, `native-tls`, or `rustls-tls(-...)`
/// feature to be enabled.
void *client_builder_tls_built_in_root_certs(void *handle,
                                                      bool tls_built_in_root_certs);

/// Controls the use of TLS server name indication.
///
/// Defaults to `true`.
void *client_builder_tls_sni(void *handle, bool tls_sni);

/// Sets the `User-Agent` header to be used by this client.
void *client_builder_user_agent(void *handle, const char *value);

/// Convenience method to make a `DELETE` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_delete(void *handle, const char *url);

void client_destroy(void *handle);

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
void *client_execute(void *handle, void *request);

/// Convenience method to make a `GET` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_get(void *handle, const char *url);

/// Convenience method to make a `HEAD` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_head(void *handle, const char *url);

/// Convenience method to make a `PATCH` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_patch(void *handle, const char *url);

/// Convenience method to make a `POST` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_post(void *handle, const char *url);

/// Convenience method to make a `PUT` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_put(void *handle, const char *url);

/// Start building a `Request` with the `Method` and `Url`.
///
/// Returns a `RequestBuilder`, which will allow setting headers and
/// request body before sending.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
void *client_request(void *handle, const char *method, const char *url);

void free_r_string(void *handle);

void free_resp_body(void *handle);

/// Inserts a key-value pair into the map.
///
/// If the map did not previously have this key present, then `false` is
/// returned.
///
/// If the map did have this key present, the new value is pushed to the end
/// of the list of values currently associated with the key. The key is not
/// updated, though; this matters for types that can be `==` without being
/// identical.
bool header_map_append(void *handle, const char *key, const char *value);

/// Returns the number of headers the map can hold without reallocating.
///
/// This number is an approximation as certain usage patterns could cause
/// additional allocations before the returned capacity is filled.
int32_t header_map_capacity(void *handle);

/// Clears the map, removing all key-value pairs. Keeps the allocated memory
/// for reuse.
void header_map_clear(void *handle);

/// Returns true if the map contains a value for the specified key.
/// Return -1 if function failed.
/// why not use bool? because bk is bool.
/// If only return false,can't show function failed or isn't contains.
bool header_map_contains_key(void *handle, const char *key);

void header_map_destroy(void *handle);

///Don't forget free
void *header_map_get(void *handle, const char *key);

/// Returns a view of all values associated with a key.
///
/// The returned view does not incur any allocations and allows iterating
/// the values associated with the key.  See [`GetAll`] for more details.
/// Returns `None` if there are no values associated with the key.
void *header_map_get_all(void *handle, const char *key);

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
bool header_map_insert(void *handle, const char *key, const char *value);

void *header_map_keys(void *handle);

/// Returns the number of keys stored in the map.
///
/// This number will be less than or equal to `len()` as each key may have
/// more than one associated value.
///
int32_t header_map_keys_len(void *handle);

/// Returns the number of headers stored in the map.
///
/// This number represents the total number of **values** stored in the map.
/// This number can be greater than or equal to the number of **keys**
/// stored given that a single key may have more than one associated value.
int32_t header_map_len(void *handle);

/// Removes a key from the map, returning the value associated with the key.
///
/// Returns `None` if the map does not contain the key. If there are
/// multiple values associated with the key, then the first one is returned.
/// See `remove_entry_mult` on `OccupiedEntry` for an API that yields all
/// values.
bool header_map_remove(void *handle, const char *key);

/// Reserves capacity for at least `additional` more headers to be inserted
/// into the `HeaderMap`.
///
/// The header map may reserve more space to avoid frequent reallocations.
/// Like with `with_capacity`, this will be a "best effort" to avoid
/// allocations until `additional` more headers are inserted. Certain usage
/// patterns could cause additional allocations before the number is
/// reached.
void header_map_reserve(void *handle, uint32_t additional);

void *header_map_values(void *handle);

void http_err_clear(void *handle);

void http_err_destroy(void *handle);

HttpErrorKind http_err_kind(void *handle);

const uint8_t *http_err_msg(void *handle);

uint64_t http_err_msg_len(void *handle);

/// Initialize the global logger and log to `rest_client.log`.
///
/// Note that this is an idempotent function, so you can call it as many
/// times as you want and logging will only be initialized the first time.
void initialize_logging();

/// Constructs a new `ClientBuilder`.
void *new_client_builder();

void *new_header_map();

/// Proxy **all** traffic to the passed URL.
void *proxy_all(const char *proxy_scheme);

void proxy_destroy(void *handle);

/// Proxy all HTTP traffic to the passed URL.
void *proxy_http(const char *proxy_scheme);

/// Proxy all HTTPS traffic to the passed URL.
void *proxy_https(const char *proxy_scheme);

const uint8_t *r_string_bytes(void *handle);

uint64_t r_string_len(void *handle);

/// Enable HTTP basic authentication.
void *request_builder_basic_auth(void *handle,
                                           const char *username,
                                           const char *password);

/// Enable HTTP bearer authentication.
void *request_builder_bearer_auth(void *handle, const char *token);

/// Set the request body from u8 array.
void *request_builder_body_bytes(void *handle,
                                           const uint8_t *bytes,
                                           uintptr_t size);

/// Set the request body from file.
void *request_builder_body_file(void *handle, const char *file_path);

/// Set the request body from file.
void *request_builder_body_file_wide(void *handle,
                                               const wchar_t *file_path,
                                               uintptr_t length);

/// Set the request body from file.
void *request_builder_body_file_with_name(void *handle,
                                                    const char *file_name,
                                                    const char *file_path);

/// Set the request body from file.
void *request_builder_body_file_with_name_wide(void *handle,
                                                         const char *file_name,
                                                         const wchar_t *file_path,
                                                         uintptr_t path_len);

/// Set the request body from UTF-8 text.
void *request_builder_body_string(void *handle, const char *str);

/// Build a `Request`, which can be inspected, modified and executed with
/// `Client::execute()`.
void *request_builder_build(void *handle);

void request_builder_destroy(void *handle);

/// Send a form body.
///
/// Sets the body to the url encoded serialization of the passed value,
/// and also sets the `Content-Type: application/x-www-form-urlencoded`
/// header.
void *request_builder_form(void *handle, const Pair *pairs, uintptr_t len);

/// Add a `Header` to this Request.
void *request_builder_header(void *handle, const char *key, const char *value);

/// Add a `Header` to this Request.
void *request_builder_headers(void *handle, void *headers);

/// Send a JSON body.
///
/// Sets the body to the JSON serialization of the passed value, and
/// also sets the `Content-Type: application/json` header.
void *request_builder_json(void *handle, const Pair *pairs, uintptr_t len);

/// Modify the query string of the URL.
///
/// Modifies the URL of this request, adding the parameters provided.
/// This method appends and does not overwrite. This means that it can
/// be called multiple times and that existing query parameters are not
/// overwritten if the same key is used. The key will simply show up
/// twice in the query string.
/// Calling `.query(&[("foo", "a"), ("foo", "b")])` gives `"foo=a&foo=b"`.
void *request_builder_query(void *handle, const Pair *querys, uintptr_t len);

/// Constructs the Request and sends it the target URL, returning a Response.
///
/// # Errors
///
/// This method fails if there was an error while sending request,
/// redirect loop was detected or redirect limit was exhausted.
void *request_builder_send(void *handle);

/// Enables a request timeout.
///
/// The timeout is applied from when the request starts connecting until the
/// response body has finished. It affects only this request and overrides
/// the timeout configured using `ClientBuilder::timeout()`.
void *request_builder_timeout(void *handle, uint64_t millisecond);

void *request_builder_try_clone(void *handle);

/// Set HTTP version
void *request_builder_version(void *handle, const char *version);

void request_destroy(void *handle);

const uint8_t *resp_body_content(void *handle);

uint64_t resp_body_content_len(void *handle);

/// Get the response text.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// Encoding is determined from the `charset` parameter of `Content-Type` header,
/// and defaults to `utf-8` if not presented.
void *response_body_text(void *handle);

/// Get the response text given a specific encoding.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// You can provide a default encoding for decoding the raw message, while the
/// `charset` parameter of `Content-Type` header is still prioritized. For more information
/// about the possible encoding name, please go to [`encoding_rs`] docs.
///
/// [`encoding_rs`]: https://docs.rs/encoding_rs/0.8/encoding_rs/#relationship-with-windows-code-pages
void *response_body_text_with_charset(void *handle,
                                          const char *default_encoding);

/// Get the full response body as `Bytes`.
/// The difference from copy_to is : This fun Consumption ownership
/// Don't forget free
void *response_bytes(void *handle);

/// Get the content-length of the response, if it is known.
uint64_t response_content_length(void *handle);

/// Copy the response body into a writer.
/// Don't forget free
///
/// This function internally uses [`std::io::copy`] and hence will continuously read data from
/// the body and then write it into writer in a streaming fashion until EOF is met.
///
/// On success, the total number of bytes that were copied to `writer` is returned.
///
/// [`std::io::copy`]: https://doc.rust-lang.org/std/io/fn.copy.html
void *response_copy_to(void *handle);

void response_destroy(void *handle);

/// Get the `Headers` of this `Response`.
void *response_headers(void *handle);

int32_t response_read(void *handle, uint8_t *buf, uint32_t buf_len);

/// Get the remote address used to get this `Response`.
void *response_remote_addr(void *handle);

/// Get the `StatusCode` of this `Response`.
int32_t response_status(void *handle);

/// Get the final `Url` of this `Response`.
void *response_url(void *handle);

/// Get the HTTP `Version` of this `Response`.
/// Don't forget free string
///Version::HTTP_09 => "HTTP/0.9",
///Version::HTTP_10 => "HTTP/1.0",
///Version::HTTP_11 => "HTTP/1.1",
///Version::HTTP_2 => "HTTP/2.0",
///Version::HTTP_3 => "HTTP/3.0",
///_ => "unreachable"
void *response_version(void *handle);

void *take_last_http_error();

}  // extern "C"

}  // namespace crab::http
