#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "log.hpp"
#include "console_commands.hpp"
#include "string_util.hpp"
#include "filetype_icon.hpp"

namespace ssf
{

class ConsolePage
{
public:
    ConsolePage()
    {
    }

    ConsolePage &set_col_width(unsigned col, unsigned width)
    {
        _col_widths.at(col) = width;
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

        // iterate over columns
        for (auto col_index = 0u; col_index < entries.size(); col_index++)
        {
            auto col = entries.at(col_index);
            // Calculate entry offset in case there are more entries
            // than rows available
            unsigned entry_offset = 0;
            if (col.size() > _row_count)
            {
                entry_offset = std::floor(_selection / _row_count) * _row_count;
            }
            int current_width = _col_widths.at(col_index);
            int current_cursor_col = 0;
            for (unsigned i = 0; i < col_index; i++)
            {
                current_cursor_col += _col_widths.at(i);
                current_cursor_col += _spacing;
            }
            // Only iterate to either row_count or size of the col
            unsigned limit = std::min((int)(col.size() - entry_offset),
                                      (int)_row_count);
            for (auto index = 0u; index < limit; index++)
            {
                // Calculate the actual index for the entry
                auto entry_index = index + entry_offset;
                auto dir_entry = col.at(entry_index);
                auto full_entry = col.at(entry_index).path().filename().string();
                auto entry = shortened_string(full_entry,
                                              current_width - 4);
                p << c_cmd::set_cursor(index, current_cursor_col);
                p << get_icon(full_entry) << " ";

                if ((entry_index == _selection && col_index == _active_col) ||
                    (entry_index == _parent_selection && col_index == _active_col - 1))
                {
                    p << c_cmd::color::background::cyan
                      << entry
                      << c_cmd::color::reset;
                }
                else
                {
                    if (dir_entry.is_directory())
                    {
                        p << c_cmd::color::foreground::bright_cyan
                          << entry
                          << c_cmd::color::reset;
                    }
                    else
                    {
                        p << entry;
                    }
                }
            }
        }
        return p.str();
    }

    void add_col(std::vector<std::filesystem::directory_entry> c)
    {
        entries.push_back(c);
    }

    void set_selection(int s)
    {
        _selection = s;
    }

    void set_parent_selection(int s)
    {
        _parent_selection = s;
    }

    void clear_entries() { entries.resize(0); }

private:
    // Settings
    unsigned _col_width = 20;
    unsigned _spacing = 2;
    unsigned _row_count = 40;

    unsigned _selection = 0;
    unsigned _parent_selection = 0;
    unsigned _active_col = 1;
    std::array<int, 3> _col_widths;
    std::vector<std::vector<std::filesystem::directory_entry>> entries;
};

} // namespace ssf
