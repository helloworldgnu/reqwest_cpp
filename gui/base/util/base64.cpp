#include "base64.h"

#include "base/internal/code_shadow.hpp"

namespace base::base64 {
ByteBuffer::uptr Decode(const uint8_t* input, uint32_t len) {
  auto handle = base::ffi::base64_decode(input, len);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr Encode(const uint8_t* input, uint32_t len) {
  auto handle = base::ffi::base64_encode(input, len);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr UrlDecode(const uint8_t* input, uint32_t len) {
  auto handle = base::ffi::base64_url_decode(input, len);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr UrlEncode(const uint8_t* input, uint32_t len) {
  auto handle = base::ffi::base64_url_encode(input, len);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}
}  // namespace base::base64