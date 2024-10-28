#pragma once

#include <memory>

namespace ffi
{

class RespRawData
{
  public:
    using uptr = std::unique_ptr<RespRawData>;

    enum class DataType
    {
        TEXT,
        BYTES,
    };

    RespRawData() = delete;
    RespRawData(const RespRawData &) = delete;
    RespRawData(RespRawData &&) = delete;
    RespRawData &operator=(const RespRawData &) = delete;
    RespRawData &operator=(RespRawData &&) = delete;

    RespRawData(DataType type, const uint8_t *data, const uint64_t len) : m_dataType(type), m_data(data), m_len(len) {};

    ~RespRawData();

    [[nodiscard]] const char *rawChar() const;
    [[nodiscard]] const uint8_t *rawBytes() const;
    [[nodiscard]] uint64_t rawBytesLength() const;

  private:
    DataType m_dataType = DataType::TEXT;
    const uint8_t *m_data;
    const uint64_t m_len;
};

} // namespace ffi