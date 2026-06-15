#pragma once

#include <string>
#include <cxxabi.h>
#include <memory>

namespace Utils
{
template <typename T>
std::string GetTypeName(T& obj);
}

template <typename T>
std::string Utils::GetTypeName(T& obj)
{
    int status = 0;
    std::unique_ptr<char, void(*)(void*)> res{abi::__cxa_demangle(typeid(obj).name(), nullptr, nullptr, &status), std::free};
    return (status == 0) ? res.get() : typeid(obj).name();
}