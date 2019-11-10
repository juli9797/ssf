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
        _selection.push_back(std::vector<std::filesystem::directory_entry>());
        _selection.push_back(std::vector<std::filesystem::directory_entry>());
        _selection.push_back(std::vector<std::filesystem::directory_entry>());

        _parent_selection.push_back(std::vector<std::filesystem::directory_entry>());
        _parent_selection.push_back(std::vector<std::filesystem::directory_entry>());
        _parent_selection.push_back(std::vector<std::filesystem::directory_entry>());
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
            std::vector<std::filesystem::directory_entry> col = entries.at(col_index);
            // Calculate entry offset in case there are more entries
            // than rows available
            unsigned entry_offset = 0;
            if (col.size() > _row_count && !_selection.empty())
            {
                entry_offset = std::floor(find_pos(col, *_selection.at(col_index).begin()) / _row_count) * _row_count;
            }
            // Handle last folder in middle column
            int current_width = _col_widths.at(col_index);
            if (col_index == 1)
            {
                if (entries.at(2).size() == 0)
                {
                    current_width += _col_widths[2];
                }
            }
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

                if (!_selection.at(col_index).empty() && (col.at(entry_index) == _selection.at(col_index).at(0) && col_index == _active_col) ||
                    (!_parent_selection.at(col_index).empty() && col.at(entry_index) == _parent_selection.at(col_index).at(0) && col_index == _active_col - 1))
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

    void clear_entries()
    {
        entries.resize(0);
    }

    void set_selection(std::filesystem::directory_entry s)
    {
        _selection.at(_active_col).resize(0);
        _selection.at(_active_col).push_back(s);
    }

    void set_parent_selection(std::filesystem::directory_entry s)
    {
        _parent_selection.at(_active_col).resize(0);
        _parent_selection.at(_active_col).push_back(s);
    }

private:
    // Settings
    unsigned _col_width = 20;
    unsigned _spacing = 2;
    unsigned _row_count = 40;

    std::vector<std::vector<std::filesystem::directory_entry>> _selection = {{}};
    std::vector<std::vector<std::filesystem::directory_entry>> _parent_selection = {{}};
    unsigned _active_col = 1;
    std::array<int, 3> _col_widths;
    std::vector<std::vector<std::filesystem::directory_entry>> entries;

    auto find_pos(const std::vector<std::filesystem::directory_entry> &dl, std::filesystem::directory_entry de) const
        -> std::size_t
    {
        auto it = std::find(dl.begin(), dl.end(), de);
        auto dist = 0;
        dist = std::distance(dl.begin(), it);
        return dist;
    }
};

} // namespace ssf
