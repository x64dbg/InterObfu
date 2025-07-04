#pragma once

#include <functional>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <climits>

// Platform-specific debug break
#ifdef _WIN32
    #define DEBUG_BREAK() __debugbreak()
#else
    #include <csignal>
    #define DEBUG_BREAK() std::raise(SIGTRAP)
    #define __debugbreak() DEBUG_BREAK()
#endif

// Platform-specific sprintf_s
#ifdef _WIN32
    // sprintf_s is available on Windows
#else
    #define sprintf_s(buffer, size, format, ...) snprintf(buffer, size, format, __VA_ARGS__)
    #define strncpy_s(dest, destSize, src, count) strncpy(dest, src, (count == _TRUNCATE) ? (destSize - 1) : count)
    #define _TRUNCATE ((size_t)-1)
    #define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// Provide constants that might be missing
#ifndef LLONG_MAX
    #define LLONG_MAX 9223372036854775807LL
#endif
#ifndef LLONG_MIN  
    #define LLONG_MIN (-9223372036854775807LL - 1)
#endif

struct State;

template<typename T>
using CompareFunction = std::function<bool(const T &, const T &, State &)>;

#define OPNEQ(T) bool operator!=(const T & other) const = delete