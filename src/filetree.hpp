#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "log.hpp"

namespace ssf
{

//
// This class is supposed to handle all the filesystem interactions using std::filesystem
//
class Filetree
{
public:
    Filetree()
    {
        log << "init path: " << std::filesystem::current_path() << "\n";
        current_path = std::filesystem::absolute(std::filesystem::current_path());
    }
    ~Filetree()
    {
    }

    auto move_layer_up()
    {
        if (current_path.has_parent_path())
        {
            log << "current Path :" << current_path << "\n";
            current_path = current_path.parent_path();
            log << "parent Path : " << current_path << "\n";
        }
    }

    void move_up()
    {
        //  Sanitize
        selection--;
    }
    void move_down()
    {
        // Sanitize
        selection++;
    }
    void move_left()
    {
        if (current_path.has_parent_path())
        {
            current_path = current_path.parent_path();
            log << "PATH " << current_path.string() << "\n";
            selection = 0;
        }
    }
    void move_right()
    {
        auto di = std::filesystem::directory_iterator(current_path);
        for (int i = 0; i < selection; i++)
        {
            di++;
        }
        auto selected = *di;
        if (selected.is_directory())
        {
            current_path = current_path / selected.path().filename();
            selection = 0;
        }
    }

    auto get_current()
    {
        auto di = std::filesystem::directory_iterator(current_path);
        std::vector<std::string> res;
        for (auto &d : di)
        {
            res.push_back(d.path().filename().string());
        }
        return res;
    }

    auto get_left()
    {
        auto temp = current_path;
        if (temp.has_parent_path())
        {
            temp = temp.parent_path();
            auto di = std::filesystem::directory_iterator(temp);
            std::vector<std::string> res;
            for (auto &d : di)
            {
                res.push_back(d.path().filename().string());
            }
            return res;
        }
        return std::vector<std::string>();
    }

    auto get_right()
    {
        auto di = std::filesystem::directory_iterator(current_path);
        for (int i = 0; i < selection; i++)
        {
            di++;
        }
        auto selected = *di;
        log << "GET RIGHT: " << selected.path().filename().string() << "\n";
        if (selected.is_directory())
        {
            auto temp_path = current_path;
            temp_path = temp_path / selected.path().filename();
            auto di = std::filesystem::directory_iterator(temp_path);
            std::vector<std::string> res;
            for (auto &d : di)
            {
                res.push_back(d.path().filename().string());
            }
            return res;
        }
        return std::vector<std::string>();
    }

private:
    int selection = 0;
    std::filesystem::path current_path;
};

} // namespace ssf