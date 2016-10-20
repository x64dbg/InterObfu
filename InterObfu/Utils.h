#pragma once

#include <functional>
#include <vector>

template<typename T>
using CompareFunction = std::function<bool(const T &, const T &)>;

#define OPNEQ(T) bool operator!=(const T & other) const { return !(*this == other); }