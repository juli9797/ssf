#pragma once

#include <vector>
#include <sstream>
#include <string>

#include "console_commands.hpp"

#include "log.hpp"

namespace ssf
{

class ConsolePage
{
public:
    ConsolePage(int space = 15) : spacing(space)
    {
    }

    auto str() const
    {
        std::ostringstream p;
        p << c_cmd::clear;

        for (auto col_index = 0u; col_index < entries.size(); col_index++)
        {
            auto col = entries.at(col_index);
            for (auto index = 0u; index < col.size(); index++)
            {
                p << c_cmd::set_cursor(index, col_index * spacing);
                if (index == selection && col_index == active_col)
                {
                    p << c_cmd::bold_enable << col.at(index) << c_cmd::bold_disable;
                }
                else
                {
                    p << col.at(index);
                }
            }
        }
        p << c_cmd::hide_cursor;
        return p.str();
    }

    void add_col(std::vector<std::string> c)
    {
        entries.push_back(c);
    }

    void selection_incr()
    {
        if (entries.size() <= active_col)
        {
            selection = 0;
        }
        if (selection < entries.at(active_col).size() - 1)
        {
            selection++;
        }
    }

    void selection_decr()
    {
        if (selection > 0)
        {
            selection--;
        }
    }

    void clear_entries()
    {
        entries.resize(0);
    }

    void reset_selection()
    {
        selection = 0;
    }

private:
    unsigned selection = 0u;
    unsigned active_col = 1;
    const int spacing;
    std::vector<std::vector<std::string>> entries;
};

} // namespace ssf