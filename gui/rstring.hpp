#pragma once

#include "client.hpp"

namespace ffi
{

struct RString
{
    const char *c_str = nullptr;

    RString(const char *const &cStr) : c_str(cStr)
    {
    }

    ~RString()
    {
        if (c_str)
        {
            free_string(c_str);
        }
    }

    std::string toStdString() const
    {
        return c_str ? std::string(c_str) : "";
    }
};
} // namespace ffi