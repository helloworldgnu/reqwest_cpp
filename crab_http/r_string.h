#pragma once
#include <memory>

namespace crab::http
{
class HeaderMap;
class Response;

class RString
{
  public:
    using uptr = std::unique_ptr<RString>;
    using sptr = std::shared_ptr<RString>;

  private:
    friend class HeaderMap;

    friend class Response;

  private:
    template <typename... Args> static std::unique_ptr<RString> Create(Args &&...args)
    {
        struct make_unique_helper : public RString
        {
            explicit make_unique_helper(Args &&...a) : RString(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    static uptr Build(void *handle);

    explicit RString(void *handle);

  public:
    RString() = delete;

    RString(const RString &) = delete;

    RString(RString &&) = delete;

    RString &operator=(const RString &) = delete;

    RString &operator=(RString &&) = delete;

    ~RString();

    [[nodiscard]] std::string Chars() const;

    [[nodiscard]] const char *CharsNonNul() const;

    [[nodiscard]] uint64_t Length() const;

  private:
    void *handle_{nullptr};
};
} // namespace crab::http
