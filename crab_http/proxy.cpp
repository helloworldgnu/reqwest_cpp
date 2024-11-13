#include "proxy.h"

#include "crab_http_c.h"

namespace crab::http
{
Proxy::Proxy(void *handle) : handle_(handle)
{
}

Proxy::~Proxy()
{
    proxy_destroy(handle_);
}

Proxy::uptr Proxy::Build(void *handle)
{
    return Create(handle);
}

Proxy::uptr Proxy::http(const std::string &proxy_scheme)
{
    auto handle = proxy_http(proxy_scheme.c_str());
    if (!handle)
    {
        return nullptr;
    }
    return Build(handle);
}

Proxy::uptr Proxy::https(const std::string &proxy_scheme)
{
    auto handle = proxy_https(proxy_scheme.c_str());
    if (!handle)
    {
        return nullptr;
    }
    return Build(handle);
}

Proxy::uptr Proxy::all(const std::string &proxy_scheme)
{
    auto handle = proxy_all(proxy_scheme.c_str());
    if (!handle)
    {
        return nullptr;
    }
    return Build(handle);
}

void *Proxy::Handle()
{
    return handle_;
}

void Proxy::ResetHandle()
{
    handle_ = nullptr;
}
} // namespace crab::http