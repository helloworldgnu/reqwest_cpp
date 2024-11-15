#pragma once

#include <memory>

namespace crab::http
{
class RequestBuilder;
class Client;

class Request
{
  public:
    using uptr = std::unique_ptr<Request>;
    using sptr = std::shared_ptr<Request>;

  private:
    friend class RequestBuilder;
    friend class Client;

  private:
    template <typename... Args> static std::unique_ptr<Request> Create(Args &&...args)
    {
        struct make_unique_helper : public Request
        {
            explicit make_unique_helper(Args &&...a) : Request(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    static uptr Build(void *handle);

    explicit Request(void *handle);

  public:
    Request() = delete;

    Request(const Request &) = delete;

    Request(Request &&) = delete;

    Request &operator=(const Request &) = delete;

    Request &operator=(Request &&) = delete;

    ~Request();

  private:
    void *Handle();

    void ResetHandle();

  private:
    void *handle_{nullptr};
};
} // namespace crab::http