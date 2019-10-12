#pragma once

#include <vector>
#include <sstream>
#include <string>

#include "console_commands.hpp"

#include "log.hpp"

namespace ssf
{

// Substring throws if length of string is shorter
// Instead, return unchanged string
auto shortened_string(std::string s, int length)
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

class SingleLine
{
public:
    SingleLine(int row) : _row(row) {}

    void clear_text()
    {
        _text = "";
    }

    SingleLine &operator<<(std::string s)
    {
    }

    auto str() const
    {
        std::ostringstream p;
        // Set Cursor
        // Clear line
        // Write new Text
        return p.str();
    }

private:
    int _row;
    std::string _text;
};

// ?TODO:
// ?Optimize to store single String only
class ConsolePage
{
public:
    ConsolePage(int col_width, int spacing) : _col_width(col_width),
                                              _spacing(spacing)

    {
    }

    auto str() const
    {
        std::ostringstream p;
        p << c_cmd::clear;
        p << c_cmd::hide_cursor;
        for (auto col_index = 0u; col_index < entries.size(); col_index++)
        {
            auto col = entries.at(col_index);
            for (auto index = 0u; index < col.size(); index++)
            {
                auto entry = shortened_string(col.at(index), _col_width);
                p << c_cmd::set_cursor(index, col_index * (_col_width + _spacing));
                if (index == selection && col_index == active_col)
                {
                    p << c_cmd::color::blue
                      << entry
                      << c_cmd::color::reset;
                }
                else
                {
                    p << entry;
                }
            }
        }
        return p.str();
    }

    void add_col(std::vector<std::string> c)
    {
        entries.push_back(c);
    }

    void set_selection(int s)
    {
        selection = s;
    }

    void clear_entries()
    {
        entries.resize(0);
    }

private:
    unsigned selection = 0;
    unsigned active_col = 1;
    const int _col_width;
    const int _spacing;
    std::vector<std::vector<std::string>> entries;
};

} // namespace ssf