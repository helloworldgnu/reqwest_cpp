#include "resp_body.h"

#include "crab_http_c.h"

namespace crab::http
{

ResponseBody::ResponseBody(void *handle) : handle_(handle)
{
}

ResponseBody::~ResponseBody()
{
    free_resp_body(handle_);
}

ResponseBody::uptr ResponseBody::Build(void *handle)
{
    return Create(handle);
}

std::string ResponseBody::Chars() const
{
    if (handle_) {
        return {CharsNonNul(), Length()};
    }
    return {};
}

const char *ResponseBody::CharsNonNul() const
{
    const auto buf = resp_body_content(handle_);
    return reinterpret_cast<const char *>(buf);
}

const uint8_t *ResponseBody::Bytes() const
{
    const auto buf = resp_body_content(handle_);
    return buf;
}

uint64_t ResponseBody::Length() const
{
    auto len = resp_body_content_len(handle_);
    return len;
}

} // namespace crab::http
