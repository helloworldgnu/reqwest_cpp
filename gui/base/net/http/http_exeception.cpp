#include "http_exeception.h"

#include "http_ffi.hpp"

namespace base::net::http {

std::string last_error_message() {
  int error_length = ffi::last_error_length();

  if (error_length == 0) {
    return {};
  }

  std::string msg(error_length, '\0');
  int ret = ffi::last_error_message(&msg[0], static_cast<int>(msg.length()));
  if (ret <= 0) {
    throw new HttpException("Fetching error message failed");
  }
  return msg;
}

HttpException HttpException::Last_error() {
  std::string msg = last_error_message();
  if (msg.empty()) {
    return HttpException("(no err available)");
  } else {
    return HttpException(msg);
  }
}
}  // namespace base::net::http