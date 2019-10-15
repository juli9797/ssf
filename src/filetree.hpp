#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <exception>
#include <functional>

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
			current_path = current_path.parent_path();
		}
	}

	void move_up()
	{
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
			else
			{
				try
				{
					move_right_on_file(selected);
				}
				catch (std::bad_function_call &e)
				{
				}
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
		}
	}

	auto get_current() const
	{
		return get_directory_list(current_path);
	}

	auto get_current_path() const
	{
		return current_path;
	}
	auto get_selection() const
	{
		return selection;
	}

	auto get_left() const
	{
		if (current_path.has_relative_path())
		{
			return get_directory_list(current_path.parent_path());
		}
		else
		{
			return std::vector<std::string>();
		}
	}

	// TODO: Remove cascaded try/catch and multiple default returns
	auto get_right() const
	{
		try
		{
			auto selected = get_directory_entry(selection);
			if (selected.is_directory())
			{
				auto temp_path = current_path / selected.path().filename();
				return get_directory_list(temp_path);
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
			// Get Dir Entry should not throw
		}
		return std::vector<std::string>();
	}

protected:
	std::function<void(std::filesystem::directory_entry)> move_right_on_file;

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
