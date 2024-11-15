#include "http_exception.h"

#include "crab_http_c.h"
#include "resp_body.h"

namespace crab::http
{
std::unique_ptr<HttpException> TakeLastError()
{
    void *handle = take_last_http_error();
    if (!handle)
    {
        return nullptr;
    }
    return HttpException::Build(handle);
}

HttpException::HttpException(void *handle) : handle_(handle)
{
}

HttpException::~HttpException()
{
    http_err_destroy(handle_);
}

HttpException::uptr HttpException::Build(void *handle)
{
    return Create(handle);
}

std::string HttpException::Chars() const
{
    if (handle_) {
        return {CharsNonNul(), Length()};
    }
    return {};
}

const char *HttpException::CharsNonNul() const
{
    const auto buf = http_err_msg(handle_);
    return reinterpret_cast<const char *>(buf);
}

uint64_t HttpException::Length() const
{
    auto len = http_err_msg_len(handle_);
    return len;
}
HttpErrorKind HttpException::ErrorKind() const
{
    auto kind = http_err_kind(handle_);
    return kind;
}

} // namespace crab::http
