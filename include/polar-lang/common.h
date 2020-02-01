#pragma once

#include <string>
#include <memory>
#include <vector>

namespace polar {

using String = std::string;

template <class T>
using U = std::unique_ptr<T>;

template <class T>
class W {
private:
    T* ptr_;
};

template <class T>
using Seq = std::vector<T>;

} // namespace polar
