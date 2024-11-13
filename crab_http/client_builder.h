#pragma once

#include <memory>
#include <string>
#include <vector>

namespace crab::http
{
class Client;
class HeaderMap;
struct Pair;
class Proxy;

class ClientBuilder
{
  public:
    using uptr = std::unique_ptr<ClientBuilder>;

  private:
    template <typename... Args> static std::unique_ptr<ClientBuilder> Create(Args &&...args)
    {
        struct make_unique_helper : public ClientBuilder
        {
            explicit make_unique_helper(Args &&...a) : ClientBuilder(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    explicit ClientBuilder(void *handle);

  public:
    ClientBuilder() = delete;

    ClientBuilder(const ClientBuilder &) = delete;

    ClientBuilder(ClientBuilder &&) = delete;

    ClientBuilder &operator=(const ClientBuilder &) = delete;

    ClientBuilder &operator=(ClientBuilder &&) = delete;

    ~ClientBuilder();

  public:
    static uptr Build();
    /// Add a custom root certificate.
    ///
    /// This allows connecting to a server that has a self-signed
    /// certificate for example. This **does not** replace the existing
    /// trusted store.
    ClientBuilder *add_root_certificate(const std::string &cert_path);

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
    ClientBuilder *danger_accept_invalid_certs(bool accept_invalid_certs);

    /// Sets the default headers for every request.
    ClientBuilder *default_headers(std::unique_ptr<HeaderMap> headermap);

    ClientBuilder *default_headers(std::initializer_list<Pair> headers);

    /// Allow HTTP/0.9 responses
    ClientBuilder *http09_responses();

    /// Set whether HTTP/1 connections will accept obsolete line folding for
    /// header values.
    ///
    /// Newline codepoints (`\r` and `\n`) will be transformed to spaces when
    /// parsing.
    ClientBuilder *http1_allow_obsolete_multiline_headers_in_responses(bool val);

    /// Only use HTTP/1.
    ClientBuilder *http1_only();

    /// Sets the maximum idle connection per host allowed in the pool.
    ClientBuilder *http1_title_case_headers();

    /// Sets whether to use an adaptive flow control.
    ///
    /// Enabling this will override the limits set in
    /// `http2_initial_stream_window_size` and
    /// `http2_initial_connection_window_size`.
    ClientBuilder *http2_adaptive_window(bool enable);

    /// Sets the max connection-level flow control for HTTP2
    ///
    /// Default is currently 65,535 but may change internally to optimize for
    /// common uses.
    ClientBuilder *http2_initial_connection_window_size(uint32_t *size = nullptr);

    ClientBuilder *http2_initial_connection_window_size(uint32_t size);

    /// Sets the `SETTINGS_INITIAL_WINDOW_SIZE` option for HTTP2 stream-level flow
    /// control.
    ///
    /// Default is currently 65,535 but may change internally to optimize for
    /// common uses.
    ClientBuilder *http2_initial_stream_window_size(uint32_t *size = nullptr);

    ClientBuilder *http2_initial_stream_window_size(uint32_t size);

    /// Sets the maximum frame size to use for HTTP2.
    ///
    /// Default is currently 16,384 but may change internally to optimize for
    /// common uses.
    ClientBuilder *http2_max_frame_size(uint32_t *size = nullptr);

    ClientBuilder *http2_max_frame_size(uint32_t size);

    /// Only use HTTP/2.
    ClientBuilder *http2_prior_knowledge();

    /// Restrict the Client to be used with HTTPS only requests.
    ///
    /// Defaults to false.
    ClientBuilder *https_only(bool enable);

    /// Bind to a local IP Address.
    ClientBuilder *local_address(const std::string &local_address);

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
    /// This requires the optional `default-tls`, `native-tls`, or
    /// `rustls-tls(-...)` feature to be enabled.
    ClientBuilder *max_tls_version(const std::string &version);

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
    /// This requires the optional `default-tls`, `native-tls`, or
    /// `rustls-tls(-...)` feature to be enabled.
    ClientBuilder *min_tls_version(const std::string &version);

    /// Disables the trust-dns async resolver.
    ///
    /// This method exists even if the optional `trust-dns` feature is not
    /// enabled. This can be used to ensure a `Client` doesn't use the trust-dns
    /// async resolver even if another dependency were to enable the optional
    /// `trust-dns` feature.
    ClientBuilder *no_trust_dns();

    /// Set an optional timeout for idle sockets being kept-alive.
    ///
    /// Pass `None` to disable timeout.
    ///
    /// Default is 90 seconds.
    ClientBuilder *pool_idle_timeout(const uint64_t *millisecond = nullptr);

    ClientBuilder *pool_idle_timeout(uint64_t millisecond);

    /// Sets the maximum idle connection per host allowed in the pool.
    ClientBuilder *pool_max_idle_per_host(uintptr_t max);

    /// Add a `Proxy` to the list of proxies the `Client` will use.
    ///
    /// # Note
    ///
    /// Adding a proxy will disable the automatic usage of the "system" proxy.
    ClientBuilder *proxy(std::unique_ptr<Proxy> proxy);

    /// Set a `redirect::Policy` for this client.
    ///
    /// Default will follow redirects up to a maximum of 10.
    ClientBuilder *redirect(uintptr_t policy);

    /// Enable or disable automatic setting of the `Referer` header.
    ///
    /// Default is `true`.
    ClientBuilder *referer(bool enable);

    /// Override DNS resolution for specific domains to a particular IP address.
    ///
    /// Warning
    ///
    /// Since the DNS protocol has no notion of ports, if you wish to send
    /// traffic to a particular port you must include this port in the URL
    /// itself, any port in the overridden addr will be ignored and traffic sent
    /// to the conventional port for the given scheme (e.g. 80 for http).
    ClientBuilder *resolve(const std::string &domain, const std::string &socket_addr);

    /// Override DNS resolution for specific domains to particular IP addresses.
    ///
    /// Warning
    ///
    /// Since the DNS protocol has no notion of ports, if you wish to send
    /// traffic to a particular port you must include this port in the URL
    /// itself, any port in the overridden addresses will be ignored and traffic
    /// sent to the conventional port for the given scheme (e.g. 80 for http).
    ClientBuilder *resolve_to_addrs(const std::string &domain, std::vector<const char *> &socket_addr_array);

    ClientBuilder *resolve_to_addrs(const std::string &domain, std::initializer_list<const char *> &socket_addr_array);

    /// Set that all sockets have `SO_KEEPALIVE` set with the supplied duration.
    ///
    /// If `None`, the option will not be set.
    ClientBuilder *tcp_keepalive(const uint64_t *millisecond = nullptr);

    ClientBuilder *tcp_keepalive(uint64_t millisecond);

    /// Set whether sockets have `TCP_NODELAY` enabled.
    ///
    /// Default is `true`.
    ClientBuilder *tcp_nodelay(bool enable);

    /// Set a timeout for connect, read and write operations of a `Client`.
    ///
    /// Default is 30 seconds.
    ///
    /// Pass `None` to disable timeout.
    ClientBuilder *timeout(const uint64_t *millisecond = nullptr);

    ClientBuilder *timeout(uint64_t millisecond);

    /// Set a timeout for connect operations of a `Client`.
    ///
    /// Default is 30 seconds.
    ///
    /// Pass `None` to disable timeout.
    ClientBuilder *connect_timeout(const uint64_t *millisecond = nullptr);

    ClientBuilder *connect_timeout(uint64_t millisecond);

    /// Controls the use of built-in system certificates during certificate
    /// validation.
    ///
    /// Defaults to `true` -- built-in system certs will be used.
    ///
    /// # Optional
    ///
    /// This requires the optional `default-tls`, `native-tls`, or
    /// `rustls-tls(-...)` feature to be enabled.
    ClientBuilder *tls_built_in_root_certs(bool tls_built_in_root_certs);

    /// Controls the use of TLS server name indication.
    ///
    /// Defaults to `true`.
    ClientBuilder *tls_sni(bool tls_sni);

    /// Sets the `User-Agent` header to be used by this client.
    ClientBuilder *user_agent(const std::string &value);

    /// Returns a `Client` that uses this `ClientBuilder` configuration.
    ///
    /// # Errors
    ///
    /// This method fails if TLS backend cannot be initialized, or the resolver
    /// cannot load the system configuration.
    std::unique_ptr<Client> build();

  private:
    void *handle_{nullptr};
};
} // namespace crab::http