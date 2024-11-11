#pragma once

#include <filesystem>
#include <memory>

#include "base/string.h"

namespace base {
class ByteBuffer;

namespace md5 {
std::unique_ptr<ByteBuffer> Encode(const uint8_t* input, uint32_t len);

std::unique_ptr<ByteBuffer> Encode(const base::String& path);

std::unique_ptr<ByteBuffer> Encode(const std::filesystem::path& path);
}  // namespace md5

namespace base64 {
std::unique_ptr<ByteBuffer> Decode(const uint8_t* input, uint32_t len);

std::unique_ptr<ByteBuffer> Encode(const uint8_t* input, uint32_t len);

std::unique_ptr<ByteBuffer> UrlDecode(const uint8_t* input, uint32_t len);

std::unique_ptr<ByteBuffer> UrlEncode(const uint8_t* input, uint32_t len);
}  // namespace base64

namespace url {
std::unique_ptr<ByteBuffer> Decode(const char* input);

std::unique_ptr<ByteBuffer> Encode(const char* input);
}  // namespace url

namespace fs {
class File;
}

namespace net::http {
class Response;
}

class ByteBuffer {
 public:
  using uptr = std::unique_ptr<ByteBuffer>;
  using sptr = std::shared_ptr<ByteBuffer>;

 private:
  friend class ::base::fs::File;

  friend class ::base::net::http::Response;

  friend ByteBuffer::uptr base::md5::Encode(const uint8_t*, uint32_t);

  friend ByteBuffer::uptr base::md5::Encode(const base::String&);

  friend ByteBuffer::uptr base::md5::Encode(const std::filesystem::path&);

  friend ByteBuffer::uptr base::base64::Decode(const uint8_t* input,
                                               uint32_t len);

  friend ByteBuffer::uptr base::base64::Encode(const uint8_t* input,
                                               uint32_t len);

  friend ByteBuffer::uptr base::base64::UrlDecode(const uint8_t* input,
                                                  uint32_t len);

  friend ByteBuffer::uptr base::base64::UrlEncode(const uint8_t* input,
                                                  uint32_t len);

  friend ByteBuffer::uptr base::url::Decode(const char*);

  friend ByteBuffer::uptr base::url::Encode(const char*);

 private:
  template <typename... Args>
  static std::unique_ptr<ByteBuffer> Create(Args&&... args) {
    struct make_unique_helper : public ByteBuffer {
      explicit make_unique_helper(Args&&... a)
          : ByteBuffer(std::forward<Args>(a)...) {}
    };
    return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
  }

 private:
  static uptr Build(void* handle);

  explicit ByteBuffer(void* handle);

 public:
  ByteBuffer() = delete;

  ByteBuffer(const ByteBuffer&) = delete;

  ByteBuffer(ByteBuffer&&) = delete;

  ByteBuffer& operator=(const ByteBuffer&) = delete;

  ByteBuffer& operator=(ByteBuffer&&) = delete;

  ~ByteBuffer();

  [[nodiscard]] const char* Chars() const;

  [[nodiscard]] const uint8_t* Bytes() const;

  [[nodiscard]] uint64_t Length() const;

 private:
  void* handle_{nullptr};
};

}  // namespace base