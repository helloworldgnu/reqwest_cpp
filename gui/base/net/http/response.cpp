#include "response.h"

#include "header_map.h"
#include "http_ffi.hpp"

namespace base::net::http {
Response::Response(void* handle) : handle_(handle) {}

Response::~Response() { ffi::response_destroy(handle_); }

Response::uptr Response::Build(void* handle) { return Create(handle); }

std::unique_ptr<ByteBuffer> Response::text() {
  auto buf = ffi::response_text(handle_);
  if (!buf) {
    return nullptr;
  }
  return ByteBuffer::Create(buf);
}

std::unique_ptr<ByteBuffer> Response::text_with_charset(
    const std::string& default_encoding) {
  auto buf = ffi::response_text_with_charset(handle_, default_encoding.c_str());
  if (!buf) {
    return nullptr;
  }

  return ByteBuffer::Create(buf);
}

std::unique_ptr<ByteBuffer> Response::bytes() {
  auto buf = ffi::response_bytes(handle_);
  if (!buf) {
    return nullptr;
  }

  return ByteBuffer::Create(buf);
}

std::unique_ptr<ByteBuffer> Response::copy_to() {
  auto buf = ffi::response_copy_to(handle_);
  if (!buf) {
    return nullptr;
  }

  return ByteBuffer::Create(buf);
}

uint64_t Response::content_length() {
  auto len = ffi::response_content_length(handle_);
  return len;
}

int32_t Response::read(uint8_t* buf, uint32_t buf_len) {
  int32_t count = ffi::response_read(handle_, buf, buf_len);
  return count;
}

std::unique_ptr<HeaderMap> Response::headers() {
  auto handle = ffi::response_headers(handle_);
  if (!handle) {
    return nullptr;
  }
  return HeaderMap::Build(handle);
}

std::string Response::remote_addr() {
  const char* _v = ffi::response_remote_addr(handle_);
  if (_v) {
    std::string _res(_v);
    ffi::free_c_string(_v);
    return _res;
  } else
    return "";
}

int32_t Response::status() { return ffi::response_status(handle_); }

std::string Response::url() {
  const char* _v = ffi::response_url(handle_);
  if (_v) {
    std::string _res(_v);
    ffi::free_c_string(_v);
    return _res;
  } else
    return "";
}

std::string Response::version() {
  const char* _v = ffi::response_version(handle_);
  if (_v) {
    std::string _res(_v);
    ffi::free_c_string(_v);
    return _res;
  } else
    return "";
}
}  // namespace base::net::http