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

// TODO:
// Add mode where all keypresses are forwarded to a command handler
// Quit this mode on key (bsp esc)

class ConsoleInputHandler
{
public:
    void loop()
    {
        while (true)
        {
            auto c = std::cin.get();
            try
            {
                callbacks.at(c)(); // Execute callback
            }
            catch (const std::out_of_range &e)
            {
                log << LogLevel::warning << "key " << (int)c << " not registered\n";
            }
            if (c == command_key)
            {
                return;
            }
        }
    }

    void register_callback(char c, std::function<void(void)> callable)
    {
        callbacks[c] = callable;
    }

private:
    std::unordered_map<char, std::function<void(void)>> callbacks;
    char command_key = ':';
};

} // namespace ssf