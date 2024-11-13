#pragma once

#include <memory>
#include <string>

namespace crab::http
{
class Response;

class ResponseBody
{
  public:
    using uptr = std::unique_ptr<ResponseBody>;
    using sptr = std::shared_ptr<ResponseBody>;

  private:
    friend class Response;

  private:
    template <typename... Args> static std::unique_ptr<ResponseBody> Create(Args &&...args)
    {
        struct make_unique_helper : public ResponseBody
        {
            explicit make_unique_helper(Args &&...a) : ResponseBody(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    static uptr Build(void *handle);

    explicit ResponseBody(void *handle);

  public:
    ResponseBody() = delete;

    ResponseBody(const ResponseBody &) = delete;

    ResponseBody(ResponseBody &&) = delete;

    ResponseBody &operator=(const ResponseBody &) = delete;

    ResponseBody &operator=(ResponseBody &&) = delete;

    ~ResponseBody();

    [[nodiscard]] std::string Chars() const;

    [[nodiscard]] const char *CharsNonNul() const;

    [[nodiscard]] const uint8_t *Bytes() const;

    [[nodiscard]] uint64_t Length() const;

  private:
    void *handle_{nullptr};
};
} // namespace crab::http