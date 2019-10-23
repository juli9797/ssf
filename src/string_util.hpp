#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace ssf
{
// Substring throws if length of string is shorter
// Instead, return unchanged string
auto shortened_string(std::string s, std::size_t length)
{
    if (s.length() < length)
    {
        return s;
    }
    else
    {
        return s.substr(0, length);
    }
}

auto to_upper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

} // namespace ssf