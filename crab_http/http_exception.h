#pragma once

#include "crab_http_c.h"
#include <memory>
#include <string>

namespace crab::http
{
class HttpException;

std::unique_ptr<HttpException> TakeLastError();

class HttpException
{
    friend std::unique_ptr<HttpException> TakeLastError();

  public:
    using uptr = std::unique_ptr<HttpException>;
    using sptr = std::shared_ptr<HttpException>;

  private:
    template <typename... Args> static std::unique_ptr<HttpException> Create(Args &&...args)
    {
        struct make_unique_helper : public HttpException
        {
            explicit make_unique_helper(Args &&...a) : HttpException(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    static uptr Build(void *handle);

    explicit HttpException(void *handle);

  public:
    HttpException() = delete;

    HttpException(const HttpException &) = delete;

    HttpException(HttpException &&) = delete;

    HttpException &operator=(const HttpException &) = delete;

    HttpException &operator=(HttpException &&) = delete;

    ~HttpException();

    [[nodiscard]] const char *CharsNonNul() const;

    [[nodiscard]] uint64_t Length() const;

    [[nodiscard]] HttpErrorKind ErrorKind() const;

  private:
    void *handle_{nullptr};
};
} // namespace crab::http
