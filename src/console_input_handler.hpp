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
    using callback_t = std::function<void(void)>;

    void process_key_press() const
    {
        auto c = read_key_blocking();
        try
        {
            callbacks.at(c)(); // Execute callback
        }
        catch (const std::out_of_range &e)
        {
            log << LogLevel::warning << "key " << c << " not registered\n";
        }
    }

    void register_callback(char c, callback_t callable)
    {
        callbacks[c] = callable;
    }

protected:
    auto read_key_blocking() const -> char
    {
        return std::cin.get();
    }

private:
    std::unordered_map<char, callback_t> callbacks;
};

} // namespace ssf