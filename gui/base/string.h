#pragma once

#include <string>
#include <utility>

#include "base/platform.h"

namespace base {

template <typename CharType>
class BasicString : public std::basic_string<CharType> {
 public:
  using SuperType = std::basic_string<CharType>;
  using SuperType::SuperType;

  BasicString(const SuperType& super) : SuperType(super) {}

  BasicString(SuperType&& super) : SuperType(std::move(super)) {}

  SuperType Native() const { return *this; }
};

#if defined(OS_OSX)
#define STR(x) x
using Char = char;
#elif defined(OS_WIN)
#define STR(x) L##x
using Char = wchar_t;
#endif

using String = BasicString<Char>;

}  // namespace base
