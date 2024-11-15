#pragma once

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace crab::http
{
class Response;
class Request;
class HeaderMap;
struct Pair;
class Client;

class RequestBuilder
{
    friend class Client;

  public:
    using uptr = std::unique_ptr<RequestBuilder>;

  private:
    template <typename... Args> static std::unique_ptr<RequestBuilder> Create(Args &&...args)
    {
        struct make_unique_helper : public RequestBuilder
        {
            explicit make_unique_helper(Args &&...a) : RequestBuilder(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    static uptr Build(void *handle);

    explicit RequestBuilder(void *handle);

  public:
    RequestBuilder() = delete;

    RequestBuilder(const RequestBuilder &) = delete;

    RequestBuilder(RequestBuilder &&) = delete;

    RequestBuilder &operator=(const RequestBuilder &) = delete;

    RequestBuilder &operator=(RequestBuilder &&) = delete;

    ~RequestBuilder();

  public:
    /// Enable HTTP basic authentication.
    RequestBuilder *basic_auth(const std::string &username, const std::string &password);

    /// Enable HTTP bearer authentication.
    RequestBuilder *bearer_auth(const std::string &token);

    /// Set the request body from u8 array.
    RequestBuilder *body(const std::vector<uint8_t> &bytes);

    /// Set the request body from file.
#if defined(_WIN32) || defined(_MSC_VER)

    RequestBuilder *file_body(const std::wstring &file_path);

#else
    RequestBuilder *file_body(const std::string &file_path);
#endif

#if defined(_WIN32) || defined(_MSC_VER)

    RequestBuilder *file_body_with_name(const std::string &file_name, const std::wstring &file_path);

#else
    RequestBuilder *file_body_with_name(const std::string &file_name, const std::string &file_path);
#endif

    /// Set the request body from UTF-8 text.
    RequestBuilder *body(const std::string &str);

    /// Send a form body.
    ///
    /// Sets the body to the url encoded serialization of the passed value,
    /// and also sets the `Content-Type: application/x-www-form-urlencoded`
    /// header.
    RequestBuilder *form(const std::vector<Pair> &querys);

    RequestBuilder *form(const std::initializer_list<Pair> &querys);

    /// Add a `Header` to this Request.
    RequestBuilder *header(const std::string &key, const std::string &value);

    /// Add a `Header` to this Request.
    RequestBuilder *headers(std::unique_ptr<HeaderMap> headers);

    /// Send a smaple JSON body.
    ///
    /// Sets the body to the JSON serialization of the passed value, and
    /// also sets the `Content-Type: application/json` header.
    RequestBuilder *json(const std::vector<Pair> &pairs);

    RequestBuilder *json(const std::initializer_list<Pair> &pairs);

    /// It is same to use header(content-type,application/json).body(json)
    RequestBuilder *json(const std::string &json);

    /// Modify the query string of the URL.
    ///
    /// Modifies the URL of this request, adding the parameters provided.
    /// This method appends and does not overwrite. This means that it can
    /// be called multiple times and that existing query parameters are not
    /// overwritten if the same key is used. The key will simply show up
    /// twice in the query string.
    /// Calling `.query(&[("foo", "a"), ("foo", "b")])` gives `"foo=a&foo=b"`.
    RequestBuilder *query(const std::vector<Pair> &querys);

    RequestBuilder *query(const std::initializer_list<Pair> &querys);

    /// Constructs the Request and sends it the target URL, returning a Response.
    ///
    /// # Errors
    ///
    /// This method fails if there was an error while sending request,
    /// redirect loop was detected or redirect limit was exhausted.
    std::unique_ptr<Response> send();

    /// Enables a request timeout.
    ///
    /// The timeout is applied from when the request starts connecting until the
    /// response body has finished. It affects only this request and overrides
    /// the timeout configured using `ClientBuilder::timeout()`.
    RequestBuilder *timeout(uint64_t millisecond);

    // if return null clone failed
    RequestBuilder *try_clone();

    /// Set HTTP version
    RequestBuilder *version(const std::string &version);

    std::unique_ptr<Request> build();

  private:
    void *handle_{nullptr};
};
} // namespace crab::http