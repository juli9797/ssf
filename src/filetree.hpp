#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <exception>
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
		if (selection != 0)
		{
			selection--;
		}
	}
	void move_down()
	{
		auto di = std::filesystem::directory_iterator(current_path);
		// Sanitize
		int dist = std::distance(std::filesystem::begin(di), std::filesystem::end(di));
		if (selection < dist - 1)
		{
			selection++;
		}
	}
	void move_left()
	{

		if (current_path.has_relative_path())
		{
			current_path = current_path.parent_path();
			selection = 0;
		}
		else
		{
			//has no relative path -> in the root dir
		}
	}
	void move_right()
	{
		try
		{
			auto selected = get_directory_entry(selection);
			if (selected.is_directory())
			{
				auto temp_path = current_path / selected.path().filename();
				auto di = std::filesystem::directory_iterator(temp_path);
				selection = 0;
				current_path = temp_path;
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
		}
	}

	auto get_current() const
	{
		try
		{
			return get_directory_list(current_path);
		}
		catch (std::filesystem::filesystem_error &e)
		{
			log << "Exception occured!!: " << e.what() << "\n";
			return std::vector<std::string>();
		}
	}
	auto get_selection() const
	{
		return selection;
	}

	auto get_left() const
	{
		auto temp = current_path;
		if (temp.has_relative_path())
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

	// TODO: Remove cascaded try/catch and multiple default returns
	auto get_right() const
	{
		try
		{
			auto selected = get_directory_entry(selection);
			log << "GET RIGHT: " << selected.path().filename().string() << "\n";

			if (selected.is_directory())
			{

				auto temp_path = current_path;
				temp_path = temp_path / selected.path().filename();
				try
				{
					return get_directory_list(temp_path);
				}
				catch (std::filesystem::filesystem_error &e)
				{
					log << "Exception occured!!: " << e.what() << "\n";
					return std::vector<std::string>();
				}
			}
			else
			{
				//ISSA FILE?
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
			log << "Exception occured!!: " << e.what() << "\n";
			auto selected = std::filesystem::directory_entry();
		}
		return std::vector<std::string>();
	}

private:
	int selection = 0;
	std::filesystem::path current_path;

	std::filesystem::directory_entry get_directory_entry(int sel) const
	{
		auto di = std::filesystem::directory_iterator(current_path);
		for (int i = 0; i < sel; i++)
		{
			di++;
		}
		return *di;
	}

	std::vector<std::string> get_directory_list(std::filesystem::path temp) const
	{
		auto di = std::filesystem::directory_iterator(temp);
		std::vector<std::string> res;
		for (auto &d : di)
		{
			res.push_back(d.path().filename().string());
		}
		return res;
	}
};

} // namespace ssf
