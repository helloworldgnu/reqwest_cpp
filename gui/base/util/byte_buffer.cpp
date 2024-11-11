#include "byte_buffer.h"

#include "base/internal/code_shadow.hpp"

namespace base {

ByteBuffer::ByteBuffer(void* handle) : handle_(handle) {}

ByteBuffer::~ByteBuffer() { base::ffi::free_byte_buffer(handle_); }

ByteBuffer::uptr ByteBuffer::Build(void* handle) { return Create(handle); }

const char* ByteBuffer::Chars() const {
  const auto buf = base::ffi::bytes_content(handle_);
  return reinterpret_cast<const char*>(buf);
}

const uint8_t* ByteBuffer::Bytes() const {
  const auto buf = base::ffi::bytes_content(handle_);
  return buf;
}

uint64_t ByteBuffer::Length() const {
  auto len = base::ffi::bytes_len(handle_);
  return len;
}

}  // namespace base
