#pragma once

#include <memory>
#include <string>

namespace base::net::http {
class RequestBuilder;

class Request;

class Response;

class ClientBuilder;

class Client {
  friend class ClientBuilder;

  friend class RequestBuilder;

 public:
  using uptr = std::unique_ptr<Client>;

 private:
  template <typename... Args>
  static std::unique_ptr<Client> Create(Args&&... args) {
    struct make_unique_helper : public Client {
      explicit make_unique_helper(Args&&... a)
          : Client(std::forward<Args>(a)...) {}
    };
    return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
  }

 private:
  explicit Client(void* handle);

 public:
  Client() = delete;

  Client(const Client&) = delete;

  Client(Client&&) = delete;

  Client& operator=(const Client&) = delete;

  Client& operator=(Client&&) = delete;

  ~Client();

 private:
  static uptr Build(void* handle);

 public:
  /// Convenience method to make a `GET` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> get(const std::string& url);

  /// Convenience method to make a `DELETE` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> delete_(const std::string& url);

  /// Convenience method to make a `HEAD` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> head(const std::string& url);

  /// Convenience method to make a `PATCH` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> patch(const std::string& url);

  /// Convenience method to make a `POST` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> post(const std::string& url);

  /// Convenience method to make a `PUT` request to a URL.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> put(const std::string& url);

  /// Start building a `Request` with the `Method` and `Url`.
  ///
  /// Returns a `RequestBuilder`, which will allow setting headers and
  /// request body before sending.
  ///
  /// # Errors
  ///
  /// This method fails whenever supplied `Url` cannot be parsed.
  std::unique_ptr<RequestBuilder> request(const std::string& method,
                                          const std::string& url);

  /// Executes a `Request`.
  ///
  /// A `Request` can be built manually with `Request::new()` or obtained
  /// from a RequestBuilder with `RequestBuilder::build()`.
  ///
  /// You should prefer to use the `RequestBuilder` and
  /// `RequestBuilder::send()`.
  ///
  /// # Errors
  ///
  /// This method fails if there was an error while sending request,
  /// or redirect limit was exhausted.
  std::unique_ptr<Response> execute(Request* request);

 private:
  void* handle_{nullptr};
};
}  // namespace base::net::http