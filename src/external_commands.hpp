#pragma once

#include <map>
#include <set>
#include <string>

namespace ssf
{
class CommandManager
{
public:
    std::map<std::string, std::set<std::string>> commands = {
        {"shell123",
         {"open", "close"}},
        {"shell",
         {"open", "close"}},
        {"shellshell",
         {"open", "close"}},
        {"test",
         {"aaa", "bbb"}}};

    // returns a set of commands that start with the provided string
    auto guess_command(std::string const partial) const
    {
        std::set<std::string> res;
        auto guess = commands.lower_bound(partial);
        while (guess != commands.end() &&
               partial == (guess->first).substr(0, partial.size()))
        {
            res.insert(guess->first);
            guess++;
        }
        return res;
    }

    // returns a set of possible arguments to the command
    // throws
    auto get_arg_list(std::string key) const
    {
        return commands.at(key);
    }
};
} // namespace ssf