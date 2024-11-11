#include "proxy.h"

#include "http_ffi.hpp"

namespace base::net::http {
Proxy::Proxy(void* handle) : handle_(handle) {}

Proxy::~Proxy() { ffi::proxy_reqwest_destroy(handle_); }

Proxy::uptr Proxy::Build(void* handle) { return Create(handle); }

Proxy::uptr Proxy::http(const std::string& proxy_scheme) {
  auto handle = ffi::proxy_reqwest_http(proxy_scheme.c_str());
  if (!handle) {
    return nullptr;
  }
  return Build(handle);
}

Proxy::uptr Proxy::https(const std::string& proxy_scheme) {
  auto handle = ffi::proxy_reqwest_https(proxy_scheme.c_str());
  if (!handle) {
    return nullptr;
  }
  return Build(handle);
}

Proxy::uptr Proxy::all(const std::string& proxy_scheme) {
  auto handle = ffi::proxy_reqwest_all(proxy_scheme.c_str());
  if (!handle) {
    return nullptr;
  }
  return Build(handle);
}

void* Proxy::handle() { return handle_; }

void Proxy::resetHandle() { handle_ = nullptr; }
}  // namespace base::net::http