#pragma once

#include <sstream>
#include <string>

namespace ssf
{
namespace c_cmd
{
constexpr auto new_line = "\r\n";
constexpr auto weak_clear = "\x1b[2J"; // doesnt really clear
constexpr auto clear = "\033c";
constexpr auto reset_cursor = "\x1b[H";
constexpr auto hide_cursor = "\x1b[?25l";
constexpr auto show_cursor = "\x1b[?25h";
constexpr auto bold_enable = "\033[1m";
constexpr auto bold_disable = "\033[0m";
constexpr auto default_start = "\x1b[?25l\x1b[H\033c";
auto set_cursor(int x, int y)
{
    std::ostringstream oss;
    oss << "\x1b[" << x + 1 << ";" << y + 1 << "H";
    return oss.str();
}
} // namespace c_cmd
} // namespace ssf