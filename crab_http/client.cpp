#include "client.h"

#include "crab_http_c.h"
#include "request.h"
#include "request_builder.h"
#include "response.h"

namespace crab::http
{
Client::Client(void *handle) : handle_(handle)
{
}

Client::~Client()
{
    client_destroy(handle_);
}

Client::uptr Client::Build(void *handle)
{
    return Create(handle);
}

std::unique_ptr<RequestBuilder> Client::get(const std::string &url)
{
    auto builder = client_get(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::delete_(const std::string &url)
{
    auto builder = client_delete(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::head(const std::string &url)
{
    auto builder = client_head(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::patch(const std::string &url)
{
    auto builder = client_patch(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::post(const std::string &url)
{
    auto builder = client_post(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::put(const std::string &url)
{
    auto builder = client_put(handle_, url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<RequestBuilder> Client::request(const std::string &method, const std::string &url)
{
    auto builder = client_request(handle_, method.c_str(), url.c_str());
    if (!builder)
    {
        return nullptr;
    }
    return RequestBuilder::Build(builder);
}

std::unique_ptr<Response> Client::execute(std::unique_ptr<Request> request)
{
    if (!handle_ || !request->Handle()) {
        return nullptr;
    }

    auto resp = client_execute(handle_, request->Handle());
    request->ResetHandle();
    if (!resp)
    {
        return nullptr;
    }

    return Response::Build(resp);
}
} // namespace crab::http