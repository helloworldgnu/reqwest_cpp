#include "response_content.h"

#include "client.hpp"

namespace ffi
{

RespRawData::~RespRawData()
{
    if (m_data)
    {
        if (m_dataType == DataType::TEXT)
        {
            free_string(reinterpret_cast<const char *>(m_data));
        }
        else
        {
            ffi::free_vec_u8(m_data, m_len);
        }
    }
}

const char *RespRawData::rawChar() const
{
    return reinterpret_cast<const char *>(m_data);
}

const uint8_t *RespRawData::rawBytes() const
{
    return m_data;
}

uint64_t RespRawData::rawBytesLength() const
{
    return m_len;
}

} // namespace ffi