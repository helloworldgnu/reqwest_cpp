#include "header_map.h"

#include "crab_http_c.h"
#include "r_string.h"

namespace crab::http
{
HeaderMap::HeaderMap(void *handle) : handle_(handle)
{
}

HeaderMap::~HeaderMap()
{
    header_map_destroy(handle_);
}

HeaderMap::uptr HeaderMap::Build()
{
    auto handle = new_header_map();
    if (!handle)
    {
        return nullptr;
    }
    return Create(handle);
}

HeaderMap::uptr HeaderMap::Build(void *handle)
{
    return Create(handle);
}

bool HeaderMap::insert(const std::string &key, const std::string &value)
{
    return header_map_insert(handle_, key.c_str(), value.c_str());
}

bool HeaderMap::append(const std::string &key, const std::string &value)
{
    return header_map_append(handle_, key.c_str(), value.c_str());
}

uintptr_t HeaderMap::capacity() const
{
    return header_map_capacity(handle_);
}

void HeaderMap::clear()
{
    header_map_clear(handle_);
}

bool HeaderMap::contains_key(const std::string &key)
{
    bool ret = header_map_contains_key(handle_, key.c_str());
    return ret;
}

std::unique_ptr<RString> HeaderMap::get(const std::string &key) const
{
    void *v = header_map_get(handle_, key.c_str());
    return RString::Build(v);
}

std::unique_ptr<RString> HeaderMap::get_all(const std::string &key) const
{
    void *v = header_map_get_all(handle_, key.c_str());
    return RString::Build(v);
}

std::unique_ptr<RString> HeaderMap::keys() const
{
    void *v = header_map_keys(handle_);
    return RString::Build(v);
}

std::unique_ptr<RString> HeaderMap::values() const
{
    void *v = header_map_values(handle_);
    return RString::Build(v);
}

uintptr_t HeaderMap::keys_len() const
{
    return header_map_keys_len(handle_);
}

uintptr_t HeaderMap::len() const
{
    return header_map_len(handle_);
}

bool HeaderMap::remove(std::string &key)
{
    return header_map_remove(handle_, key.c_str());
}

void HeaderMap::reserve(uint32_t additional)
{
    return header_map_reserve(handle_, additional);
}

void HeaderMap::ResetHandle()
{
    handle_ = nullptr;
}

void *HeaderMap::Handle() const
{
    return handle_;
}

} // namespace crab::http