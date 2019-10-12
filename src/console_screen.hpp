#pragma once

#include <iostream>
#include <string>
#include <functional>

#include "console_commands.hpp"
#include "log.hpp"

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
        try
        {
            post_draw_callback();
        }
        catch (std::bad_function_call &c)
        {
            log << c.what();
        }
        return *this;
    }

    // Adds a string like operator<< but does not call post_draw_callback
    // use within post_draw_callback
    // !!! Terrible architecture
    // !!! Needs refractoring
    void add(std::string s)
    {
        std::cout << s;
    }

    template <typename Callable>
    void set_post_draw_callback(Callable c)
    {
        post_draw_callback = c;
    }

private:
    std::function<void(void)> post_draw_callback;
};

} // namespace ssf