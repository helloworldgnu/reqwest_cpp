#pragma once

// Operating system
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if defined(TARGET_OS_OSX)
#define OS_OSX 1
#endif
#elif defined(_WIN32)
#define OS_WIN 1
#else
#error Unsupported operating system
#endif

// Compiler
#if defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#else
#error Unsupported compiler
#endif

// Compiler specific
#if defined(COMPILER_CLANG)
    #define READABLE_FUNCTION __PRETTY_FUNCTION__
#elif defined(COMPILER_MSVC)
    #define READABLE_FUNCTION __FUNCTION__
#endif
