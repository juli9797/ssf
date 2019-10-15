#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>

#include "log.hpp"

namespace ssf
{

constexpr char ctrl_key(char const c)
{
    return c & 0x1f;
}

class ConsoleInputHandler
{
public:
    void loop_until(char exit_char)
    {
        char c = '\0';
        while (c != exit_char)
        {
            c = std::cin.get();
            try
            {
                callbacks.at(c)(); // Execute callback
            }
            catch (const std::out_of_range &e)
            {
                log << "key " << (int)c << " not registered\n";
            }
        }
    }

    void register_callback(char c, std::function<void(void)> callable)
    {
        callbacks[c] = callable;
    }

private:
    std::unordered_map<char, std::function<void(void)>> callbacks;
};

} // namespace ssf