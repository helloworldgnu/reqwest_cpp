#include "client.h"

#include "http_ffi.hpp"
#include "request_builder.h"
#include "response.h"

namespace base::net::http {
Client::Client(void* handle) : handle_(handle) {}

Client::~Client() { ffi::client_destroy(handle_); }

Client::uptr Client::Build(void* handle) { return Create(handle); }

std::unique_ptr<RequestBuilder> Client::get(const std::string& url) {
  auto builder = ffi::client_get(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::delete_(const std::string& url) {
  auto builder = ffi::client_delete(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::head(const std::string& url) {
  auto builder = ffi::client_head(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::patch(const std::string& url) {
  auto builder = ffi::client_patch(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::post(const std::string& url) {
  auto builder = ffi::client_post(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::put(const std::string& url) {
  auto builder = ffi::client_put(handle_, url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::request(const std::string& method,
                                                const std::string& url) {
  auto builder = ffi::client_request(handle_, method.c_str(), url.c_str());
  if (!builder) {
    return nullptr;
  }
  return RequestBuilder::Build(builder);
}

std::unique_ptr<Response> Client::execute(Request* request) {
  auto resp = ffi::client_execute(handle_, request);
  if (!resp) {
    return nullptr;
  }
  return Response::Build(resp);
}
}  // namespace base::net::http