#include "request.h"

#include "crab_http_c.h"

namespace crab::http
{
Request::Request(void *handle) : handle_(handle)
{
}

Request::~Request()
{
    request_destroy(handle_);
}

Request::uptr Request::Build(void *handle)
{
    return Create(handle);
}

void *Request::Handle()
{
    return handle_;
}

void Request::ResetHandle()
{
    handle_ = nullptr;
}

} // namespace crab::http
