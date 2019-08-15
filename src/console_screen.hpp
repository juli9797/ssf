#pragma once

#include <iostream>
#include <string>

#include "console_commands.hpp"

namespace ssf
{

class ConsoleScreen
{
public:
    ConsoleScreen()
    {
        *this << c_cmd::hide_cursor;
    }
    ~ConsoleScreen()
    {
        *this << c_cmd::clear
              << c_cmd::reset_cursor; // Clear screen on exit
    }
    ConsoleScreen const &operator<<(std::string const buffer) const
    {
        std::cout << buffer;
        return *this;
    }
};

} // namespace ssf