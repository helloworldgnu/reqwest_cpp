#include "response.h"

#include "crab_http_c.h"
#include "header_map.h"
#include "r_string.h"

namespace crab::http
{
Response::Response(void *handle) : handle_(handle)
{
}

Response::~Response()
{
    response_destroy(handle_);
}

Response::uptr Response::Build(void *handle)
{
    return Create(handle);
}

std::unique_ptr<ResponseBody> Response::bodyText()
{
    auto buf = response_body_text(handle_);
    if (!buf)
    {
        return nullptr;
    }
    return ResponseBody::Create(buf);
}

std::unique_ptr<ResponseBody> Response::bodyTextWithCharset(const std::string &default_encoding)
{
    auto buf = response_body_text_with_charset(handle_, default_encoding.c_str());
    if (!buf)
    {
        return nullptr;
    }

    return ResponseBody::Create(buf);
}

std::unique_ptr<ResponseBody> Response::bodyBytes()
{
    auto buf = response_bytes(handle_);
    if (!buf)
    {
        return nullptr;
    }

    return ResponseBody::Create(buf);
}

std::unique_ptr<ResponseBody> Response::copy_to()
{
    auto buf = response_copy_to(handle_);
    if (!buf)
    {
        return nullptr;
    }

    return ResponseBody::Create(buf);
}

uint64_t Response::content_length()
{
    auto len = response_content_length(handle_);
    return len;
}

int32_t Response::read(uint8_t *buf, uint32_t buf_len)
{
    int32_t count = response_read(handle_, buf, buf_len);
    return count;
}

std::unique_ptr<HeaderMap> Response::headers()
{
    auto handle = response_headers(handle_);
    if (!handle)
    {
        return nullptr;
    }
    return HeaderMap::Build(handle);
}

std::unique_ptr<RString> Response::remote_addr()
{
    void *v = response_remote_addr(handle_);
    return RString::Build(v);
}

int32_t Response::status()
{
    return response_status(handle_);
}

std::unique_ptr<RString> Response::url()
{
    void *v = response_url(handle_);
    return RString::Build(v);
}

std::unique_ptr<RString> Response::version()
{
    void *v = response_version(handle_);
    return RString::Build(v);
}
} // namespace crab::http