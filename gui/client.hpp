#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>
#include "ffi.hpp"

namespace ffi {

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
ClientBuilder *client_builder_add_root_certificate(ClientBuilder *client_builder,
                                                   const char *cert_path);

/// Returns a `Client` that uses this `ClientBuilder` configuration.
///
/// # Errors
///
/// This method fails if TLS backend cannot be initialized, or the resolver
/// cannot load the system configuration.
Client *client_builder_build_client(ClientBuilder *client_builder);

/// Set a timeout for connect operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
ClientBuilder *client_builder_connect_timeout(ClientBuilder *client_builder,
                                              const uint64_t *millisecond);

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
ClientBuilder *client_builder_danger_accept_invalid_certs(ClientBuilder *client_builder,
                                                          bool accept_invalid_certs);

/// Sets the default headers for every request.
ClientBuilder *client_builder_default_headers(ClientBuilder *client_builder, HeaderMap *headermap);

///Generally not required
void client_builder_destroy(ClientBuilder *client_builder);

/// Allow HTTP/0.9 responses
ClientBuilder *client_builder_http09_responses(ClientBuilder *client_builder);

/// Set whether HTTP/1 connections will accept obsolete line folding for
/// header values.
///
/// Newline codepoints (`\r` and `\n`) will be transformed to spaces when
/// parsing.
ClientBuilder *client_builder_http1_allow_obsolete_multiline_headers_in_responses(ClientBuilder *client_builder,
                                                                                  bool val);

/// Only use HTTP/1.
ClientBuilder *client_builder_http1_only(ClientBuilder *client_builder);

/// Sets the maximum idle connection per host allowed in the pool.
ClientBuilder *client_builder_http1_title_case_headers(ClientBuilder *client_builder);

/// Sets whether to use an adaptive flow control.
///
/// Enabling this will override the limits set in `http2_initial_stream_window_size` and
/// `http2_initial_connection_window_size`.
ClientBuilder *client_builder_http2_adaptive_window(ClientBuilder *client_builder, bool enable);

/// Sets the max connection-level flow control for HTTP2
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
ClientBuilder *client_builder_http2_initial_connection_window_size(ClientBuilder *client_builder,
                                                                   uint32_t *size);

/// Sets the `SETTINGS_INITIAL_WINDOW_SIZE` option for HTTP2 stream-level flow control.
///
/// Default is currently 65,535 but may change internally to optimize for common uses.
ClientBuilder *client_builder_http2_initial_stream_window_size(ClientBuilder *client_builder,
                                                               uint32_t *size);

/// Sets the maximum frame size to use for HTTP2.
///
/// Default is currently 16,384 but may change internally to optimize for common uses.
ClientBuilder *client_builder_http2_max_frame_size(ClientBuilder *client_builder, uint32_t *size);

/// Only use HTTP/2.
ClientBuilder *client_builder_http2_prior_knowledge(ClientBuilder *client_builder);

/// Restrict the Client to be used with HTTPS only requests.
///
/// Defaults to false.
ClientBuilder *client_builder_https_only(ClientBuilder *client_builder, bool enable);

/// Bind to a local IP Address.
ClientBuilder *client_builder_local_address(ClientBuilder *client_builder,
                                            const char *local_address);

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
ClientBuilder *client_builder_max_tls_version(ClientBuilder *client_builder, const char *version);

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
ClientBuilder *client_builder_min_tls_version(ClientBuilder *client_builder, const char *version);

/// Disables the trust-dns async resolver.
///
/// This method exists even if the optional `trust-dns` feature is not enabled.
/// This can be used to ensure a `Client` doesn't use the trust-dns async resolver
/// even if another dependency were to enable the optional `trust-dns` feature.
ClientBuilder *client_builder_no_trust_dns(ClientBuilder *client_builder);

/// Set an optional timeout for idle sockets being kept-alive.
///
/// Pass `None` to disable timeout.
///
/// Default is 90 seconds.
ClientBuilder *client_builder_pool_idle_timeout(ClientBuilder *client_builder,
                                                const uint64_t *millisecond);

/// Sets the maximum idle connection per host allowed in the pool.
ClientBuilder *client_builder_pool_max_idle_per_host(ClientBuilder *client_builder, uintptr_t max);

/// Add a `Proxy` to the list of proxies the `Client` will use.
///
/// # Note
///
/// Adding a proxy will disable the automatic usage of the "system" proxy.
ClientBuilder *client_builder_proxy(ClientBuilder *client_builder, Proxy *proxy);

/// Set a `redirect::Policy` for this client.
///
/// Default will follow redirects up to a maximum of 10.
ClientBuilder *client_builder_redirect(ClientBuilder *client_builder, uintptr_t policy);

/// Enable or disable automatic setting of the `Referer` header.
///
/// Default is `true`.
ClientBuilder *client_builder_referer(ClientBuilder *client_builder, bool enable);

/// Override DNS resolution for specific domains to a particular IP address.
///
/// Warning
///
/// Since the DNS protocol has no notion of ports, if you wish to send
/// traffic to a particular port you must include this port in the URL
/// itself, any port in the overridden addr will be ignored and traffic sent
/// to the conventional port for the given scheme (e.g. 80 for http).
ClientBuilder *client_builder_resolve(ClientBuilder *client_builder,
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
ClientBuilder *client_builder_resolve_to_addrs(ClientBuilder *client_builder,
                                               const char *domain,
                                               const char *const *socket_addr_array,
                                               uintptr_t len);

/// Set that all sockets have `SO_KEEPALIVE` set with the supplied duration.
///
/// If `None`, the option will not be set.
ClientBuilder *client_builder_tcp_keepalive(ClientBuilder *client_builder,
                                            const uint64_t *millisecond);

/// Set whether sockets have `TCP_NODELAY` enabled.
///
/// Default is `true`.
ClientBuilder *client_builder_tcp_nodelay(ClientBuilder *client_builder, bool enable);

/// Set a timeout for connect, read and write operations of a `Client`.
///
/// Default is 30 seconds.
///
/// Pass `None` to disable timeout.
ClientBuilder *client_builder_timeout(ClientBuilder *client_builder, const uint64_t *millisecond);

/// Controls the use of built-in system certificates during certificate validation.
///
/// Defaults to `true` -- built-in system certs will be used.
///
/// # Optional
///
/// This requires the optional `default-tls`, `native-tls`, or `rustls-tls(-...)`
/// feature to be enabled.
ClientBuilder *client_builder_tls_built_in_root_certs(ClientBuilder *client_builder,
                                                      bool tls_built_in_root_certs);

/// Controls the use of TLS server name indication.
///
/// Defaults to `true`.
ClientBuilder *client_builder_tls_sni(ClientBuilder *client_builder, bool tls_sni);

/// Sets the `User-Agent` header to be used by this client.
ClientBuilder *client_builder_user_agent(ClientBuilder *client_builder, const char *value);

/// Convenience method to make a `DELETE` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_delete(Client *client, const char *url);

void client_destroy(Client *client);

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
Response *client_execute(Client *client, Request *request);

/// Convenience method to make a `GET` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_get(Client *client, const char *url);

/// Convenience method to make a `HEAD` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_head(Client *client, const char *url);

/// Convenience method to make a `PATCH` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_patch(Client *client, const char *url);

/// Convenience method to make a `POST` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_post(Client *client, const char *url);

/// Convenience method to make a `PUT` request to a URL.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_put(Client *client, const char *url);

/// Start building a `Request` with the `Method` and `Url`.
///
/// Returns a `RequestBuilder`, which will allow setting headers and
/// request body before sending.
///
/// # Errors
///
/// This method fails whenever supplied `Url` cannot be parsed.
RequestBuilder *client_request(Client *client, const char *method, const char *url);

void free_string(const char *s);

void free_vec_u8(const uint8_t *s, uintptr_t len);

/// Inserts a key-value pair into the map.
///
/// If the map did not previously have this key present, then `false` is
/// returned.
///
/// If the map did have this key present, the new value is pushed to the end
/// of the list of values currently associated with the key. The key is not
/// updated, though; this matters for types that can be `==` without being
/// identical.
bool header_map_append(HeaderMap *header_map, const char *key, const char *value);

/// Returns the number of headers the map can hold without reallocating.
///
/// This number is an approximation as certain usage patterns could cause
/// additional allocations before the returned capacity is filled.
uintptr_t header_map_capacity(const HeaderMap *header_map);

/// Clears the map, removing all key-value pairs. Keeps the allocated memory
/// for reuse.
void header_map_clear(HeaderMap *header_map);

/// Returns true if the map contains a value for the specified key.
/// Return -1 if function failed.
/// why not use bool? because bk is bool.
/// If only return false,can't show function failed or isn't contains.
int32_t header_map_contains_key(const HeaderMap *header_map, const char *key, bool *bk);

void header_map_destroy(const HeaderMap *header_map);

///Don't forget free
const char *header_map_get(const HeaderMap *header_map, const char *key);

/// Returns a view of all values associated with a key.
///
/// The returned view does not incur any allocations and allows iterating
/// the values associated with the key.  See [`GetAll`] for more details.
/// Returns `None` if there are no values associated with the key.
const char *header_map_get_all(const HeaderMap *header_map, const char *key);

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
bool header_map_insert(HeaderMap *header_map, const char *key, const char *value);

const char *header_map_keys(const HeaderMap *header_map);

/// Returns the number of keys stored in the map.
///
/// This number will be less than or equal to `len()` as each key may have
/// more than one associated value.
///
uintptr_t header_map_keys_len(const HeaderMap *header_map);

/// Returns the number of headers stored in the map.
///
/// This number represents the total number of **values** stored in the map.
/// This number can be greater than or equal to the number of **keys**
/// stored given that a single key may have more than one associated value.
uintptr_t header_map_len(const HeaderMap *header_map);

/// Removes a key from the map, returning the value associated with the key.
///
/// Returns `None` if the map does not contain the key. If there are
/// multiple values associated with the key, then the first one is returned.
/// See `remove_entry_mult` on `OccupiedEntry` for an API that yields all
/// values.
bool header_map_remove(HeaderMap *header_map, const char *key);

/// Reserves capacity for at least `additional` more headers to be inserted
/// into the `HeaderMap`.
///
/// The header map may reserve more space to avoid frequent reallocations.
/// Like with `with_capacity`, this will be a "best effort" to avoid
/// allocations until `additional` more headers are inserted. Certain usage
/// patterns could cause additional allocations before the number is
/// reached.
void header_map_reserve(HeaderMap *header_map, uintptr_t additional);

const char *header_map_values(const HeaderMap *header_map);

/// Initialize the global logger and log to `rest_client.log`.
///
/// Note that this is an idempotent function, so you can call it as many
/// times as you want and logging will only be initialized the first time.
void initialize_logging();

/// Calculate the number of bytes in the last error's error message **not**
/// including any trailing `null` characters.
int last_error_length();

/// Write the most recent error message into a caller-provided buffer as a UTF-8
/// string, returning the number of bytes written.
///
/// # Note
///
/// This writes a **UTF-8** string into the buffer. Windows users may need to
/// convert it to a UTF-16 "unicode" afterwards.
///
/// If there are no recent errors then this returns `0` (because we wrote 0
/// bytes). `-1` is returned if there are any errors, for example when passed a
/// null pointer or a buffer of insufficient size.
int last_error_message(char *buffer, int length);

/// Constructs a new `ClientBuilder`.
ClientBuilder *new_client_builder();

HeaderMap *new_header_map();

/// Proxy **all** traffic to the passed URL.
Proxy *proxy_reqwest_all(const char *proxy_scheme);

void proxy_reqwest_destroy(Proxy *p);

/// Proxy all HTTP traffic to the passed URL.
Proxy *proxy_reqwest_http(const char *proxy_scheme);

/// Proxy all HTTPS traffic to the passed URL.
Proxy *proxy_reqwest_https(const char *proxy_scheme);

/// Enable HTTP basic authentication.
RequestBuilder *request_builder_basic_auth(RequestBuilder *request_builder,
                                           const char *username,
                                           const char *password);

/// Enable HTTP bearer authentication.
RequestBuilder *request_builder_bearer_auth(RequestBuilder *request_builder, const char *token);

/// Set the request body from u8 array.
RequestBuilder *request_builder_body_bytes(RequestBuilder *request_builder,
                                           const uint8_t *bytes,
                                           uintptr_t size);

/// Set the request body from file.
RequestBuilder *request_builder_body_file(RequestBuilder *request_builder, const char *file_path);

/// Set the request body from file.
RequestBuilder *request_builder_body_file_with_name(RequestBuilder *request_builder,
                                                    const char *file_name,
                                                    const char *file_path);

/// Set the request body from UTF-8 text.
RequestBuilder *request_builder_body_string(RequestBuilder *request_builder, const char *str);

/// Build a `Request`, which can be inspected, modified and executed with
/// `Client::execute()`.
Request *request_builder_build(RequestBuilder *request_builder);

void request_builder_destroy(RequestBuilder *request_builder);

/// Send a form body.
///
/// Sets the body to the url encoded serialization of the passed value,
/// and also sets the `Content-Type: application/x-www-form-urlencoded`
/// header.
RequestBuilder *request_builder_form(RequestBuilder *request_builder,
                                     const Pair *pairs,
                                     uintptr_t len);

/// Add a `Header` to this Request.
RequestBuilder *request_builder_header(RequestBuilder *request_builder,
                                       const char *key,
                                       const char *value);

/// Add a `Header` to this Request.
RequestBuilder *request_builder_headers(RequestBuilder *request_builder, HeaderMap *headers);

/// Send a JSON body.
///
/// Sets the body to the JSON serialization of the passed value, and
/// also sets the `Content-Type: application/json` header.
RequestBuilder *request_builder_json(RequestBuilder *request_builder,
                                     const Pair *pairs,
                                     uintptr_t len);

/// Modify the query string of the URL.
///
/// Modifies the URL of this request, adding the parameters provided.
/// This method appends and does not overwrite. This means that it can
/// be called multiple times and that existing query parameters are not
/// overwritten if the same key is used. The key will simply show up
/// twice in the query string.
/// Calling `.query(&[("foo", "a"), ("foo", "b")])` gives `"foo=a&foo=b"`.
RequestBuilder *request_builder_query(RequestBuilder *request_builder,
                                      const Pair *querys,
                                      uintptr_t len);

/// Constructs the Request and sends it the target URL, returning a Response.
///
/// # Errors
///
/// This method fails if there was an error while sending request,
/// redirect loop was detected or redirect limit was exhausted.
Response *request_builder_send(RequestBuilder *request_builder, uint32_t *kind, int32_t *value);

/// Enables a request timeout.
///
/// The timeout is applied from when the request starts connecting until the
/// response body has finished. It affects only this request and overrides
/// the timeout configured using `ClientBuilder::timeout()`.
RequestBuilder *request_builder_timeout(RequestBuilder *request_builder, uint64_t millisecond);

RequestBuilder *request_builder_try_clone(RequestBuilder *request_builder);

/// Set HTTP version
RequestBuilder *request_builder_version(RequestBuilder *request_builder, const char *version);

/// Get the full response body as `Bytes`.
/// The difference from copy_to is : This fun Consumption ownership
/// Don't forget free
const uint8_t *response_bytes(Response *response, uint32_t *kind, int32_t *value);

/// todo extensions.
/// Get the content-length of the response, if it is known.
const uint64_t *response_content_length(Response *response);

void response_content_length_destroy(uint64_t *content_length);

/// Copy the response body into a writer.
/// Don't forget free
///
/// This function internally uses [`std::io::copy`] and hence will continuously read data from
/// the body and then write it into writer in a streaming fashion until EOF is met.
///
/// On success, the total number of bytes that were copied to `writer` is returned.
///
/// [`std::io::copy`]: https://doc.rust-lang.org/std/io/fn.copy.html
const uint8_t *response_copy_to(Response *response);

void response_destroy(Response *response);

/// Get the `Headers` of this `Response`.
const HeaderMap *response_headers(Response *response);

int32_t response_read(Response *response, uint8_t *buf, uint32_t buf_len, uint32_t *kind);

/// Get the remote address used to get this `Response`.
const char *response_remote_addr(Response *response);

/// Get the `StatusCode` of this `Response`.
uint16_t response_status(Response *response);

/// Get the response text.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// Encoding is determined from the `charset` parameter of `Content-Type` header,
/// and defaults to `utf-8` if not presented.
const char *response_text(Response *response, uint32_t *kind, int32_t *value);

/// Get the response text given a specific encoding.
///
/// This method decodes the response body with BOM sniffing
/// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
/// You can provide a default encoding for decoding the raw message, while the
/// `charset` parameter of `Content-Type` header is still prioritized. For more information
/// about the possible encoding name, please go to [`encoding_rs`] docs.
///
/// [`encoding_rs`]: https://docs.rs/encoding_rs/0.8/encoding_rs/#relationship-with-windows-code-pages
const char *response_text_with_charset(Response *response,
                                       const char *default_encoding);

/// Get the final `Url` of this `Response`.
const char *response_url(Response *response);

/// Get the HTTP `Version` of this `Response`.
/// Don't forget free string
///Version::HTTP_09 => "HTTP/0.9",
///Version::HTTP_10 => "HTTP/1.0",
///Version::HTTP_11 => "HTTP/1.1",
///Version::HTTP_2 => "HTTP/2.0",
///Version::HTTP_3 => "HTTP/3.0",
///_ => "unreachable"
const char *response_version(Response *response);

}  // extern "C"

}  // namespace ffi
