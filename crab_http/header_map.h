#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace crab::http
{
class RString;
class ClientBuilder;
class RequestBuilder;

class HeaderMap
{
    friend class ClientBuilder;

    friend class RequestBuilder;

  public:
    using uptr = std::unique_ptr<HeaderMap>;

  private:
    template <typename... Args> static std::unique_ptr<HeaderMap> Create(Args &&...args)
    {
        struct make_unique_helper : public HeaderMap
        {
            explicit make_unique_helper(Args &&...a) : HeaderMap(std::forward<Args>(a)...)
            {
            }
        };
        return std::make_unique<make_unique_helper>(std::forward<Args>(args)...);
    }

  private:
    explicit HeaderMap(void *handle);

  public:
    HeaderMap() = delete;

    HeaderMap(const HeaderMap &) = delete;

    HeaderMap(HeaderMap &&) = delete;

    HeaderMap &operator=(const HeaderMap &) = delete;

    HeaderMap &operator=(HeaderMap &&) = delete;

    ~HeaderMap();

  public:
    static uptr Build();

    static uptr Build(void *handle);

    bool insert(const std::string &key, const std::string &value);

    /// Inserts a key-value pair into the map.
    ///
    /// If the map did not previously have this key present, then `false` is
    /// returned.
    ///
    /// If the map did have this key present, the new value is pushed to the end
    /// of the list of values currently associated with the key. The key is not
    /// updated, though; this matters for types that can be `==` without being
    /// identical.
    bool append(const std::string &key, const std::string &value);

    /// Returns the number of headers the map can hold without reallocating.
    ///
    /// This number is an approximation as certain usage patterns could cause
    /// additional allocations before the returned capacity is filled.
    uintptr_t capacity() const;

    /// Clears the map, removing all key-value pairs. Keeps the allocated memory
    /// for reuse.
    void clear();

    /// bk true if the map contains a value for the specified key.
    /// Return -1 if function failed.
    /// why not use bool? because bk is bool.
    /// If only return false,can't show function failed or isn't contains.
    bool contains_key(const std::string &key);

    std::unique_ptr<RString> get(const std::string &key) const;

    /// Returns a view of all values associated with a key.
    ///
    /// The returned view does not incur any allocations and allows iterating
    /// the values associated with the key.  See [`GetAll`] for more details.
    /// Returns `None` if there are no values associated with the key.
    std::unique_ptr<RString> get_all(const std::string &key) const;

    std::unique_ptr<RString> keys() const;

    /// Returns the number of keys stored in the map.
    ///
    /// This number will be less than or equal to `len()` as each key may have
    /// more than one associated value.
    ///
    uintptr_t keys_len() const;

    /// Returns the number of headers stored in the map.
    ///
    /// This number represents the total number of **values** stored in the map.
    /// This number can be greater than or equal to the number of **keys**
    /// stored given that a single key may have more than one associated value.
    uintptr_t len() const;

    /// Removes a key from the map, returning the value associated with the key.
    ///
    /// Returns `None` if the map does not contain the key. If there are
    /// multiple values associated with the key, then the first one is returned.
    /// See `remove_entry_mult` on `OccupiedEntry` for an API that yields all
    /// values.
    bool remove(std::string &key);

    /// Reserves capacity for at least `additional` more headers to be inserted
    /// into the `HeaderMap`.
    ///
    /// The header map may reserve more space to avoid frequent reallocations.
    /// Like with `with_capacity`, this will be a "best effort" to avoid
    /// allocations until `additional` more headers are inserted. Certain usage
    /// patterns could cause additional allocations before the number is
    /// reached.
    void reserve(uint32_t additional);

    std::unique_ptr<RString> values() const;

  private:
    void ResetHandle();

    void *Handle() const;

  private:
    void *handle_{nullptr};
};
} // namespace crab::http