#pragma once

#include <functional>
#include <vector>

struct State;

template<typename T>
using CompareFunction = std::function<bool(const T &, const T &, State &)>;

#define OPNEQ(T) bool operator!=(const T & other) const = delete