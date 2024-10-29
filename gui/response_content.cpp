#include "response_content.h"

#include "client.hpp"

namespace ffi
{

RespRaw::~RespRaw()
{
    if (data_)
    {
        ffi::free_vec_u8(data_, len_);
    }
}

const char *RespRaw::rawChar() const
{
    return reinterpret_cast<const char *>(data_);
}

const uint8_t *RespRaw::rawBytes() const
{
    return data_;
}

uint64_t RespRaw::rawBytesLength() const
{
    return len_;
}

} // namespace ffi