#pragma once

#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

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

class ConsolePage
{
public:
    ConsolePage()
    {
    }

    ConsolePage &set_col_width(unsigned val)
    {
        _col_width = val;
        return *this;
    }

    ConsolePage &set_spacing(unsigned val)
    {
        _spacing = val;
        return *this;
    }

    ConsolePage &set_row_count(unsigned val)
    {
        _row_count = val;
        return *this;
    }

    auto str() const
    {
        std::ostringstream p;
        p << c_cmd::clear;
        p << c_cmd::hide_cursor;
        for (auto col_index = 0u; col_index < entries.size(); col_index++)
        {
            auto col = entries.at(col_index);
            unsigned entry_offset = 0;
            if (col.size() > _row_count)
            {
                entry_offset = (_selection / _row_count) * _row_count;
            }
            unsigned limit = std::min((int)col.size(), (int)_row_count);
            for (auto index = 0u; index < limit; index++)
            {
                auto entry_index = index + entry_offset;
                auto entry = shortened_string(col.at(entry_index), _col_width);
                p << c_cmd::set_cursor(index, col_index * (_col_width + _spacing));
                if (entry_index == _selection && col_index == _active_col)
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
        _selection = s;
    }

    void clear_entries()
    {
        entries.resize(0);
    }

private:
    // Settings
    unsigned _col_width = 20;
    unsigned _spacing = 2;
    unsigned _row_count = 40;

    unsigned _selection = 0;
    unsigned _active_col = 1;
    std::vector<std::vector<std::string>> entries;
};

} // namespace ssf