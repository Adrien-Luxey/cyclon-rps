#ifndef UTILS_ERROR_HPP_
#define UTILS_ERROR_HPP_

// Taken from: https://github.com/Loki-Astari/Examples/tree/master/Version2

#include <string>
#include <sstream>
#include <utility>
#include <cstddef>

template<typename... Args>
int print(std::ostream& s, Args&... args)
{
    using Expander = int[];
    return Expander{ 0, ((s << std::forward<Args>(args)), 0)...}[0];
}

template<typename... Args>
std::string buildStringFromParts(Args const&... args)
{
    std::stringstream msg;
    print(msg, args...);
    return msg.str();
}

template<typename... Args>
std::string buildErrorMessage(Args const&... args)
{
    return buildStringFromParts(args...);
}

#endif // UTILS_ERROR_HPP_

