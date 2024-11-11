#include "md5.h"

#include "base/internal/code_shadow.hpp"

namespace base::md5 {

ByteBuffer::uptr Encode(const uint8_t* input, uint32_t len) {
  auto handle = base::ffi::md5_bytes(input, len);
  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr Encode(const base::String& path) {
  void* handle = nullptr;
  const auto p = path.Native();

#if OS_WIN
  handle = base::ffi::md5_file_wide(p.c_str(), p.size());
#else
  handle = base::ffi::md5_file(p.c_str());
#endif

  if (!handle) {
    return nullptr;
  }
  return ByteBuffer::Create(handle);
}

ByteBuffer::uptr Encode(const std::filesystem::path& path) {
  const auto& p = path.native();
  return Encode(base::String(p));
}

}  // namespace base::md5