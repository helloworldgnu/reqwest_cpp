#pragma once

#include <exception>
#include <string>

namespace base::net::http {

    class HttpException : std::exception {
    public:
        explicit HttpException(const std::string &msg) : msg(msg) {};

        static HttpException Last_error();

        const char *what() const throw() {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}