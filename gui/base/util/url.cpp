#include "url.h"

#include "base/internal/code_shadow.hpp"

namespace base::url {
ByteBuffer::uptr Decode(const char* input) {
  auto handle = base::ffi::url_decode(input);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr Encode(const char* input) {
  auto handle = base::ffi::url_encode(input);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

}  // namespace base::url