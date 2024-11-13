#include "r_string.h"

#include "crab_http_c.h"

namespace crab::http
{

RString::RString(void *handle) : handle_(handle)
{
}

RString::~RString()
{
    free_r_string(handle_);
}

RString::uptr RString::Build(void *handle)
{
    return Create(handle);
}

const char *RString::CharsNonNul() const
{
    const auto buf = r_string_bytes(handle_);
    return reinterpret_cast<const char *>(buf);
}

uint64_t RString::Length() const
{
    auto len = r_string_len(handle_);
    return len;
}

} // namespace crab::http
