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
constexpr auto clear_line = "\033[2K";
constexpr auto reset_cursor = "\x1b[H";
constexpr auto hide_cursor = "\x1b[?25l";
constexpr auto show_cursor = "\x1b[?25h";
constexpr auto bold_enable = "\033[1m";
constexpr auto bold_disable = "\033[0m";
constexpr auto default_start = "\x1b[?25l\x1b[H\033c";

// more colors https://en.wikipedia.org/wiki/ANSI_escape_code
namespace color
{
constexpr auto reset = "\u001b[0m";

namespace background
{
constexpr auto blue = "\u001b[44m";
constexpr auto red = "\u001b[41m";
constexpr auto yellow = "\u001b[43m";
constexpr auto cyan = "\u001b[46m";
constexpr auto white = "\u001b[47m";
constexpr auto bright_cyan = "\u001b[106m";
} // namespace background

namespace foreground
{
constexpr auto red = "\u001b[31m";
constexpr auto yellow = "\u001b[33m";
constexpr auto blue = "\u001b[34m";
constexpr auto bright_magenta = "\u001b[95m";
constexpr auto bright_cyan = "\u001b[96m";
} // namespace foreground
} // namespace color

auto set_cursor(int x, int y)
{
    std::ostringstream oss;
    oss << "\x1b[" << x + 1 << ";" << y + 1 << "H";
    return oss.str();
}
} // namespace c_cmd
} // namespace ssf