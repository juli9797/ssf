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
    void process_key_press()
    {
        auto c = read_key_blocking();
        if (char_forward_mode == true)
        {
            if (c == char_forward_cancel_key)
            {
                char_forward_mode = false;
                try
                {
                    char_forward_complete_callback();
                }
                catch (std::bad_function_call &c)
                {
                    log << c.what();
                }
            }
            else
            {
                try
                {
                    char_forward_callback(c);
                }
                catch (std::bad_function_call &c)
                {
                    log << c.what();
                }
            }
        }
        else
        {
            try
            {
                callbacks.at(c)(); // Execute callback
            }
            catch (const std::out_of_range &e)
            {
                log << LogLevel::warning << "key " << (int)c << " not registered\n";
            }
        }
    }

    void register_callback(char c, std::function<void(void)> callable)
    {
        callbacks[c] = callable;
    }

    template <typename Callable>
    void forward_all(Callable c)
    {
        char_forward_mode = true;
        char_forward_callback = c;
    }

    template <typename Callable>
    void set_char_forward_complete_cb(Callable c)
    {
        char_forward_complete_callback = c;
    }

    void cancel_forwarding()
    {
        char_forward_mode = false;
    }

protected:
    auto read_key_blocking() const -> char
    {
        return std::cin.get();
    }

private:
    std::unordered_map<char, std::function<void(void)>> callbacks;
    std::function<void(char)> char_forward_callback;
    std::function<void()> char_forward_complete_callback;
    bool char_forward_mode = false;
    char char_forward_cancel_key = 27;
};

} // namespace ssf