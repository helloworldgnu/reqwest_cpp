#pragma once

#include <memory>
#include <string>

namespace base::net::http {
class ClientBuilder;

class Proxy {
  friend class ClientBuilder;

 public:
  using uptr = std::unique_ptr<Proxy>;

 private:
  template <typename... Args>
  static std::unique_ptr<Proxy> Create(Args&&... args) {
    struct make_unique_helper : public Proxy {
      explicit make_unique_helper(Args&&... a)
          : Proxy(std::forward<Args>(a)...) {}
    };
    return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
  }

 private:
  static uptr Build(void* handle);

  explicit Proxy(void* handle);

 public:
  Proxy() = delete;

  Proxy(const Proxy&) = delete;

  Proxy(Proxy&&) = delete;

  Proxy& operator=(const Proxy&) = delete;

  Proxy& operator=(Proxy&&) = delete;

  ~Proxy();

 public:
  static Proxy::uptr http(const std::string& proxy_scheme);

  static Proxy::uptr https(const std::string& proxy_scheme);

  static Proxy::uptr all(const std::string& proxy_scheme);

 private:
  void* handle();
  void resetHandle();

 private:
  void* handle_{nullptr};
};  // namespace proxy
}  // namespace base::net::http