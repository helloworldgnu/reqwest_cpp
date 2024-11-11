#include "client_builder.h"

#include "client.h"
#include "header_map.h"
#include "http_ffi.hpp"
#include "pair.h"
#include "proxy.h"

namespace base::net::http {
ClientBuilder::ClientBuilder(void* handle) : handle_(handle) {}

ClientBuilder::~ClientBuilder() { ffi::client_builder_destroy(handle_); }

ClientBuilder::uptr ClientBuilder::Build() {
  auto builder = ffi::new_client_builder();
  if (!builder) {
    return nullptr;
  }
  return Create(builder);
}

ClientBuilder* ClientBuilder::add_root_certificate(
    const std::string& cert_path) {
  auto builder =
      ffi::client_builder_add_root_certificate(handle_, cert_path.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::danger_accept_invalid_certs(
    bool accept_invalid_certs) {
  auto builder = ffi::client_builder_danger_accept_invalid_certs(
      handle_, accept_invalid_certs);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::default_headers(
    std::unique_ptr<HeaderMap> headerMap) {
  auto builder =
      ffi::client_builder_default_headers(handle_, headerMap->handle());
  if (builder) {
    handle_ = builder;
    headerMap->resetHandle();
  }
  return this;
}

ClientBuilder* ClientBuilder::default_headers(
    std::initializer_list<Pair> headers) {
  HeaderMap::uptr headerMap = HeaderMap::Build();
  for (auto& item : headers) {
    headerMap->insert(item.key, item.value);
  }
  auto builder =
      ffi::client_builder_default_headers(handle_, headerMap->handle());
  if (builder) {
    handle_ = builder;
    headerMap->resetHandle();
  }
  return this;
}

ClientBuilder* ClientBuilder::http09_responses() {
  auto builder = ffi::client_builder_http09_responses(handle_);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder*
ClientBuilder::http1_allow_obsolete_multiline_headers_in_responses(bool val) {
  auto builder =
      ffi::client_builder_http1_allow_obsolete_multiline_headers_in_responses(
          handle_, val);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http1_only() {
  auto builder = ffi::client_builder_http1_only(handle_);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http1_title_case_headers() {
  auto builder = ffi::client_builder_http1_title_case_headers(handle_);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_adaptive_window(bool enable) {
  auto builder = ffi::client_builder_http2_adaptive_window(handle_, enable);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_initial_connection_window_size(
    uint32_t* size) {
  auto builder =
      ffi::client_builder_http2_initial_connection_window_size(handle_, size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_initial_connection_window_size(
    uint32_t size) {
  auto builder =
      ffi::client_builder_http2_initial_connection_window_size(handle_, &size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_initial_stream_window_size(uint32_t* size) {
  auto builder =
      ffi::client_builder_http2_initial_stream_window_size(handle_, size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_initial_stream_window_size(uint32_t size) {
  auto builder =
      ffi::client_builder_http2_initial_stream_window_size(handle_, &size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_max_frame_size(uint32_t* size) {
  auto builder = ffi::client_builder_http2_max_frame_size(handle_, size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_max_frame_size(uint32_t size) {
  auto builder = ffi::client_builder_http2_max_frame_size(handle_, &size);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::http2_prior_knowledge() {
  auto builder = ffi::client_builder_http2_prior_knowledge(handle_);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::https_only(bool enable) {
  auto builder = ffi::client_builder_https_only(handle_, enable);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::local_address(const std::string& local_address) {
  auto builder =
      ffi::client_builder_local_address(handle_, local_address.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::max_tls_version(const std::string& version) {
  auto builder = ffi::client_builder_max_tls_version(handle_, version.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::min_tls_version(const std::string& version) {
  auto builder = ffi::client_builder_min_tls_version(handle_, version.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::no_trust_dns() {
  auto builder = ffi::client_builder_no_trust_dns(handle_);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::pool_idle_timeout(const uint64_t* millisecond) {
  auto builder = ffi::client_builder_pool_idle_timeout(handle_, millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::pool_idle_timeout(const uint64_t millisecond) {
  auto builder = ffi::client_builder_pool_idle_timeout(handle_, &millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::pool_max_idle_per_host(uintptr_t max) {
  auto builder = ffi::client_builder_pool_max_idle_per_host(handle_, max);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::proxy(std::unique_ptr<Proxy> proxy) {
  auto builder = ffi::client_builder_proxy(handle_, proxy->handle());
  if (builder) {
    handle_ = builder;
    proxy->resetHandle();
  }
  return this;
}

ClientBuilder* ClientBuilder::redirect(size_t max) {
  auto builder = ffi::client_builder_redirect(handle_, max);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::referer(bool enable) {
  auto builder = ffi::client_builder_referer(handle_, enable);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::resolve(const std::string& domain,
                                      const std::string& socket_addr) {
  auto builder =
      ffi::client_builder_resolve(handle_, domain.c_str(), socket_addr.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::resolve_to_addrs(
    const std::string& domain, std::vector<const char*>& socket_addr_array) {
  auto builder = ffi::client_builder_resolve_to_addrs(
      handle_, domain.c_str(), &socket_addr_array[0], socket_addr_array.size());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::resolve_to_addrs(
    const std::string& domain,
    std::initializer_list<const char*>& socket_addr_array) {
  std::vector<const char*> tmp(socket_addr_array);
  return this->resolve_to_addrs(domain, tmp);
}

ClientBuilder* ClientBuilder::tcp_keepalive(const uint64_t* millisecond) {
  auto builder = ffi::client_builder_tcp_keepalive(handle_, millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::tcp_keepalive(const uint64_t millisecond) {
  auto builder = ffi::client_builder_tcp_keepalive(handle_, &millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::tcp_nodelay(bool enable) {
  auto builder = ffi::client_builder_tcp_nodelay(handle_, enable);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::timeout(const uint64_t* millisecond) {
  auto builder = ffi::client_builder_timeout(handle_, millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::timeout(const uint64_t millisecond) {
  auto builder = ffi::client_builder_timeout(handle_, &millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::connect_timeout(const uint64_t* millisecond) {
  auto builder = ffi::client_builder_connect_timeout(handle_, millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::connect_timeout(const uint64_t millisecond) {
  auto builder = ffi::client_builder_connect_timeout(handle_, &millisecond);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::tls_built_in_root_certs(
    bool tls_built_in_root_certs) {
  auto builder = ffi::client_builder_tls_built_in_root_certs(
      handle_, tls_built_in_root_certs);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::tls_sni(bool tls_sni) {
  auto builder = ffi::client_builder_tls_sni(handle_, tls_sni);
  if (builder) {
    handle_ = builder;
  }
  return this;
}

ClientBuilder* ClientBuilder::user_agent(const std::string& value) {
  auto builder = ffi::client_builder_user_agent(handle_, value.c_str());
  if (builder) {
    handle_ = builder;
  }
  return this;
}

std::unique_ptr<Client> ClientBuilder::build() {
  auto client = ffi::client_builder_build_client(handle_);
  if (!client) {
    return nullptr;
  }
  return Client::Build(client);
}
}  // namespace base::net::http