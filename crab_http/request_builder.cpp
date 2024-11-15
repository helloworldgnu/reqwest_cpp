#include "request_builder.h"

#include "crab_http_c.h"
#include "header_map.h"
#include "request.h"
#include "response.h"

namespace crab::http
{
RequestBuilder::RequestBuilder(void *handle) : handle_(handle)
{
}

RequestBuilder::~RequestBuilder()
{
    request_builder_destroy(handle_);
}

RequestBuilder::uptr RequestBuilder::Build(void *handle)
{
    return Create(handle);
}

RequestBuilder *RequestBuilder::basic_auth(const std::string &username, const std::string &password)
{
    auto builder = request_builder_basic_auth(handle_, username.c_str(), password.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::bearer_auth(const std::string &token)
{
    auto builder = request_builder_bearer_auth(handle_, token.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::body(const std::vector<uint8_t> &bytes)
{
    auto builder = request_builder_body_bytes(handle_, &bytes[0], bytes.size());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

#if defined(_WIN32) || defined(_MSC_VER)

RequestBuilder *RequestBuilder::file_body(const std::wstring &file_path)
{
    auto builder = request_builder_body_file_wide(handle_, file_path.c_str(), file_path.size());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

#else
RequestBuilder *RequestBuilder::file_body(const std::string &file_path)
{
    auto builder = request_builder_body_file(handle_, file_path.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}
#endif

#if defined(_WIN32) || defined(_MSC_VER)

RequestBuilder *RequestBuilder::file_body_with_name(const std::string &file_name, const std::wstring &file_path)
{
    auto builder = request_builder_body_file_with_name_wide(handle_, file_name.c_str(), file_path.c_str(), file_path.size());

    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

#else
RequestBuilder *RequestBuilder::file_body_with_name(const std::string &file_name, const std::string &file_path)
{
    auto builder = request_builder_body_file_with_name(handle_, file_name.c_str(), file_path.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}
#endif

RequestBuilder *RequestBuilder::body(const std::string &str)
{
    auto builder = request_builder_body_string(handle_, str.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::form(const std::vector<Pair> &pairs)
{
    auto builder = request_builder_form(handle_, &pairs[0], pairs.size());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::form(const std::initializer_list<Pair> &pairs)
{
    std::vector<Pair> tmp(pairs);
    return this->form(tmp);
}

RequestBuilder *RequestBuilder::header(const std::string &key, const std::string &value)
{
    auto builder = request_builder_header(handle_, key.c_str(), value.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::headers(std::unique_ptr<HeaderMap> headers)
{
    auto builder = request_builder_headers(handle_, headers->Handle());
    if (builder)
    {
        handle_ = builder;
        headers->ResetHandle();
    }
    return this;
}

RequestBuilder *RequestBuilder::json(const std::vector<Pair> &pairs)
{
    auto builder = request_builder_json(handle_, &pairs[0], pairs.size());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::json(const std::initializer_list<Pair> &pairs)
{
    std::vector<Pair> tmp(pairs);
    return this->json(tmp);
}

RequestBuilder *RequestBuilder::json(const std::string &json)
{
    header("content-type", "application/json");
    body(json);
    return this;
}

RequestBuilder *RequestBuilder::query(const std::vector<Pair> &querys)
{
    auto builder = request_builder_query(handle_, &querys[0], querys.size());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::query(const std::initializer_list<Pair> &querys)
{
    std::vector<Pair> tmp(querys);
    return this->query(tmp);
}

std::unique_ptr<Response> RequestBuilder::send()
{
    if (!handle_) {
        return nullptr;
    }

    auto resp = request_builder_send(handle_);
    handle_ = nullptr;

    if (!resp)
    {
        return nullptr;
    }
    return Response::Build(resp);
}

RequestBuilder *RequestBuilder::timeout(uint64_t millisecond)
{
    auto builder = request_builder_timeout(handle_, millisecond);
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::try_clone()
{
    auto builder = request_builder_try_clone(handle_);
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

RequestBuilder *RequestBuilder::version(const std::string &version)
{
    auto builder = request_builder_version(handle_, version.c_str());
    if (builder)
    {
        handle_ = builder;
    }
    return this;
}

std::unique_ptr<Request> RequestBuilder::build()
{
    if (!handle_)
    {
        return nullptr;
    }

    auto req = request_builder_build(handle_);
    handle_ = nullptr;

    if (!req)
    {
        return nullptr;
    }

    return Request::Build(req);
}
} // namespace crab::http