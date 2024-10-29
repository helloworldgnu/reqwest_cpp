#pragma once

#include <memory>

namespace ffi
{

class RespRaw
{
  public:
    using uptr = std::unique_ptr<RespRaw>;

  private:
    friend class Response;

  private:
    RespRaw(const uint8_t *data, const uint64_t len) : data_(data), len_(len) {};

    template <typename... Args> static std::unique_ptr<RespRaw> create(Args &&...args)
    {
        struct make_unique_helper : public RespRaw
        {
            explicit make_unique_helper(Args &&...a) : RespRaw(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  public:
    RespRaw() = delete;
    RespRaw(const RespRaw &) = delete;
    RespRaw(RespRaw &&) = delete;
    RespRaw &operator=(const RespRaw &) = delete;
    RespRaw &operator=(RespRaw &&) = delete;

    ~RespRaw();

    [[nodiscard]] const char *rawChar() const;
    [[nodiscard]] const uint8_t *rawBytes() const;
    [[nodiscard]] uint64_t rawBytesLength() const;

  private:
    const uint8_t *data_;
    const uint64_t len_;
};

} // namespace ffi