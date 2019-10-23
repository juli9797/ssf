#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <cctype>

#include "console_commands.hpp"
#include "string_util.hpp"

namespace ssf
{
static std::unordered_map<std::string, std::string> icon_colors = {
    {"jpg", c_cmd::color::foreground::blue},
    {"txt", c_cmd::color::foreground::yellow},
    {"pdf", c_cmd::color::foreground::red}};

std::string get_icon(std::string file)
{
    std::ostringstream oss;
    auto index = file.find_last_of(".");
    if (index != std::string::npos)
    {
        auto filetype = file.substr(index + 1);

        auto it = icon_colors.find(filetype);
        if (it != icon_colors.end())
        {
            oss << it->second;
        }
        else
        {
            //oss << c_cmd::color::background::cyan;
        }
        auto icon_text = shortened_string(filetype, 3);
        while (icon_text.length() < 3)
        {
            icon_text.push_back(' ');
        }
        oss << to_upper(icon_text)
            << c_cmd::color::reset;
    }
    else
    {
        oss << c_cmd::color::foreground::bright_cyan
            << "---"
            << c_cmd::color::reset;
    }
    return oss.str();
}
} // namespace ssf