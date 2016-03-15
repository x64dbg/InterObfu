#ifndef UTILS_H
#define UTILS_H

#include <functional>

template<typename T>
using CompareFunction = std::function<bool(const T &, const T &)>;

#define OPNEQ(T) bool operator!=(const T & other) const { return !(*this == other); }

#endif //UTILS_H