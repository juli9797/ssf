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
        current_path = std::filesystem::current_path(); //initialize current path
        current_path = std::filesystem::absolute(current_path);
    }
    ~Filetree()
    {
        log << "filetree DESTROYED!!!11elf"
            << "\n";
    }
    //
    // This function obtains the directory contents relative to the working directory
    //
    auto get_folder_contents(int layer = 0)
    {
        log << "get folder Path: "
            << "\n";
        std::filesystem::path path = current_path;
        for (int i = 0; i <= layer; i++)
        {
            ssf::log << path << "\n";
            if (path.has_parent_path())
            {
                path = path.parent_path();
            }
            else
            {
                //idk what to do here
            }
        }

        auto di = std::filesystem::directory_iterator(path);
        //[DEBUG] list file names
        for (auto &p : di)
        {
            log << p << "\n";
        }
        return di;
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
    auto move_to_dir(std::filesystem::path dir)
    {
    }

    void move_up() {}
    void move_down() {}
    void move_left() {}
    void move_right() {}

    auto get_current()
    {
        return std::vector<std::string>({"Folder1", "Folder2"});
    }

    auto get_left()
    {
        return std::vector<std::string>({"Parent1", "Parent2"});
    }

    auto get_right()
    {
        return std::vector<std::string>({"Child1", "Child2"});
    }

private:
    // selection
    std::filesystem::path current_path;
};

} // namespace ssf