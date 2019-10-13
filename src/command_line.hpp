#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>

#include "console_commands.hpp"

#include "log.hpp"

namespace ssf
{

class CommandLine
{
public:
    CommandLine() {}

    CommandLine &set_row(unsigned row)
    {
        _row = row;
        return *this;
    }

    void clear_text()
    {
        _text = "";
    }

    CommandLine &operator<<(std::string s)
    {
        _text += s;
        return *this;
    }

    void loop()
    {
        while (true)
        {
            char c = std::cin.get();
            switch (c)
            {
            case 127: // Delete
                if (_text.length())
                {
                    _text.pop_back();
                }
                break;
            case 13: // Enter
                try
                {
                    command_cb(_text.substr(1, _text.length()));
                }
                catch (std::bad_function_call &c)
                {
                    log << c.what();
                }
                return;
                break;
            case 27: // ESC
                return;
                break;
            default:
                _text.push_back(c);
                break;
            }
            try
            {
                draw_cb(str());
            }
            catch (std::bad_function_call &e)
            {
                log << "CommandLine Draw Callback Missing";
            }
        }
    }

    std::string str() const
    {
        std::ostringstream p;
        p << c_cmd::set_cursor(_row, 0)
          << c_cmd::clear_line
          << _text;
        return p.str();
    }

    template <typename Callable>
    void set_command_cb(Callable c)
    {
        command_cb = c;
    }

    template <typename Callable>
    void set_draw_cb(Callable c)
    {
        draw_cb = c;
    }

private:
    int _row;
    std::string _text;

    std::function<void(std::string)> command_cb;
    std::function<void(std::string)> draw_cb;
};
} // namespace ssf