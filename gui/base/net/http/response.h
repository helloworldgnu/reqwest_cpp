#pragma once

#include <memory>

#include "base/util/byte_buffer.h"

namespace base::net::http {
class HeaderMap;

class RequestBuilder;

class Client;

class Response {
  friend class RequestBuilder;

  friend class Client;

 public:
  using uptr = std::unique_ptr<Response>;

 private:
  template <typename... Args>
  static std::unique_ptr<Response> Create(Args&&... args) {
    struct make_unique_helper : public Response {
      explicit make_unique_helper(Args&&... a)
          : Response(std::forward<Args>(a)...) {}
    };
    return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
  }

 private:
  static uptr Build(void* handle);

  explicit Response(void* handle);

 public:
  Response() = delete;

  Response(const Response&) = delete;

  Response(Response&&) = delete;

  Response& operator=(const Response&) = delete;

  Response& operator=(Response&&) = delete;

  ~Response();

 public:
  // This fun Consumption ownership
  /// Get the response text.
  ///
  /// This method decodes the response body with BOM sniffing
  /// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
  /// Encoding is determined from the `charset` parameter of `Content-Type`
  /// header, and defaults to `utf-8` if not presented.
  std::unique_ptr<ByteBuffer> text();

  /// Get the response text given a specific encoding.
  ///
  /// This method decodes the response body with BOM sniffing
  /// and with malformed sequences replaced with the REPLACEMENT CHARACTER.
  /// You can provide a default encoding for decoding the raw message, while the
  /// `charset` parameter of `Content-Type` header is still prioritized. For
  /// more information about the possible encoding name, please go to
  /// [`encoding_rs`] docs.
  ///
  /// [`encoding_rs`]:
  /// https://docs.rs/encoding_rs/0.8/encoding_rs/#relationship-with-windows-code-pages
  std::unique_ptr<ByteBuffer> text_with_charset(
      const std::string& default_encoding);

  /// Get the full response body as `Bytes`.
  /// The difference from copy_to is : This fun Consumption ownership
  std::unique_ptr<ByteBuffer> bytes();

  /// Copy the response body into a writer.
  /// Don't forget free
  ///
  /// This function internally uses [`std::io::copy`] and hence will
  /// continuously read data from the body and then write it into writer in a
  /// streaming fashion until EOF is met.
  ///
  /// On success, the total number of bytes that were copied to `writer` is
  /// returned.
  ///
  /// [`std::io::copy`]: https://doc.rust-lang.org/std/io/fn.copy.html
  std::unique_ptr<ByteBuffer> copy_to();

  /// Get the content-length of the response, if it is known.
  uint64_t content_length();

  int32_t read(uint8_t* buf, uint32_t buf_len);

  /// Get the `Headers` of this `Response`.
  std::unique_ptr<HeaderMap> headers();

  /// Get the remote address used to get this `Response`.
  std::string remote_addr();

  /// Get the `StatusCode` of this `Response`.
  int32_t status();

  /// Get the final `Url` of this `Response`.
  std::string url();

  /// Get the HTTP `Version` of this `Response`.
  /// Don't forget free string
  /// Version::HTTP_09 => "HTTP/0.9",
  /// Version::HTTP_10 => "HTTP/1.0",
  /// Version::HTTP_11 => "HTTP/1.1",
  /// Version::HTTP_2 => "HTTP/2.0",
  /// Version::HTTP_3 => "HTTP/3.0",
  ///_ => "unreachable"
  std::string version();

 private:
  void* handle_{nullptr};
};
}  // namespace base::net::http