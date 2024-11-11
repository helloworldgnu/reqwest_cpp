#include "header_map.h"

#include "http_ffi.hpp"

namespace base::net::http {
HeaderMap::HeaderMap(void* handle) : handle_(handle) {}

HeaderMap::~HeaderMap() { ffi::header_map_destroy(handle_); }

HeaderMap::uptr HeaderMap::Build() {
  auto handle = ffi::new_header_map();
  if (!handle) {
    return nullptr;
  }
  return Create(handle);
}

HeaderMap::uptr HeaderMap::Build(void* handle) { return Create(handle); }

bool HeaderMap::insert(const std::string& key, const std::string& value) {
  return ffi::header_map_insert(handle_, key.c_str(), value.c_str());
}

bool HeaderMap::append(const std::string& key, const std::string& value) {
  return ffi::header_map_append(handle_, key.c_str(), value.c_str());
}

uintptr_t HeaderMap::capacity() const {
  return ffi::header_map_capacity(handle_);
}

void HeaderMap::clear() { ffi::header_map_clear(handle_); }

bool HeaderMap::contains_key(const std::string& key) {
  bool ret = ffi::header_map_contains_key(handle_, key.c_str());
  return ret;
}

std::string HeaderMap::get(const std::string& key) const {
  auto value = ffi::header_map_get(handle_, key.c_str());
  if (value) {
    std::string _res(value);
    ffi::free_c_string(value);
    return _res;
  } else
    return "";
}

std::string HeaderMap::get_all(const std::string& key) const {
  auto value = ffi::header_map_get_all(handle_, key.c_str());
  if (value) {
    std::string _res(value);
    ffi::free_c_string(value);
    return _res;
  } else
    return "";
}

std::string HeaderMap::keys() const {
  auto value = ffi::header_map_keys(handle_);
  if (value) {
    std::string _res(value);
    ffi::free_c_string(value);
    return _res;
  } else
    return "";
}

std::string HeaderMap::values() const {
  auto value = ffi::header_map_values(handle_);
  if (value) {
    std::string _res(value);
    ffi::free_c_string(value);
    return _res;
  } else
    return "";
}

uintptr_t HeaderMap::keys_len() const {
  return ffi::header_map_keys_len(handle_);
}

uintptr_t HeaderMap::len() const { return ffi::header_map_len(handle_); }

bool HeaderMap::remove(std::string& key) {
  return ffi::header_map_remove(handle_, key.c_str());
}

void HeaderMap::reserve(uintptr_t additional) {
  return ffi::header_map_reserve(handle_, additional);
}

void HeaderMap::resetHandle() { handle_ = nullptr; }

void* HeaderMap::handle() const { return handle_; }

}  // namespace base::net::http