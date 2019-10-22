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

	void move_up()
	{
		if (selection != 0)
		{
			selection--;
		}
	}
	void move_down()
	{
		auto dist = get_directory_list(current_path).size();
		if (selection < dist - 1)
		{
			selection++;
		}
		else
		{
			selection = dist - 1;
		}
	}
	void move_left()
	{

		if (current_path.has_parent_path())
		{
			selection = get_parent_selection();
			current_path = current_path.parent_path();
		}
	}
	void move_right()
	{
		try
		{
			auto selected = get_directory_entry(selection);
			if (selected.is_directory())
			{
				auto probePerm = get_directory_list(get_selected_path()); //probe if filesystem throws permission error
				current_path = get_selected_path();
				selection = 0;
			}
			else if (selected.is_regular_file() || selected.is_character_file())
			{
				try
				{
					move_right_on_file();
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

	auto get_selected_path() const -> std::filesystem::path
	{
		return get_directory_entry(selection).path();
	}
	//mod this
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
	auto get_parent_selection() -> int
	{
		if (current_path.has_parent_path())
		{
			auto parent_dir = get_directory_list(current_path.parent_path());
			auto dir_name = current_path.filename();
			auto it = std::find_if(parent_dir.begin(), parent_dir.end(),
								   [&](std::filesystem::directory_entry &dir) {
									   return dir.path().filename() == dir_name;
								   });
			if (it != parent_dir.end())
			{
				return std::distance(parent_dir.begin(), it);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	//and this
	auto get_left() const
	{
		if (current_path.has_relative_path())
		{
			return get_directory_list(current_path.parent_path());
		}
		else
		{
			return std::vector<std::filesystem::directory_entry>();
		}
	}

	// TODO: Remove cascaded try/catch and multiple default returns
	auto get_right() const
	{
		try
		{
			if (get_directory_list(current_path).size() != 0)
			{
				auto selected = get_directory_entry(selection);
				if (selected.is_directory() && !std::filesystem::is_empty(selected))
				{
					return get_directory_list(get_selected_path());
				}
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
			// Get Dir Entry should not throw
		}

		return std::vector<std::filesystem::directory_entry>();
	}
	void set_hide_dot_files(bool val)
	{
		hide_dot_files = val;
		//fix selection
		auto dist = get_directory_list(current_path).size();
		if (!(selection <= dist - 1))
		{
			selection = dist - 1;
		}
	}
	auto get_hide_dot_files() -> bool
	{
		return hide_dot_files;
	}

	template <typename Callable>
	void set_move_right_on_file_cb(Callable c)
	{
		move_right_on_file = c;
	}

	std::size_t get_entry_count() const
	{
		auto di = std::filesystem::directory_iterator(current_path);
		return std::distance(std::filesystem::begin(di), std::filesystem::end(di));
	}

private:
	unsigned long int selection = 0;
	bool hide_dot_files = true;
	std::filesystem::path current_path;

	std::function<void(void)> move_right_on_file;

	std::filesystem::directory_entry get_directory_entry(unsigned sel) const
	{
		return get_directory_list(current_path).at(sel);
	}

	std::vector<std::filesystem::directory_entry> get_directory_list(std::filesystem::path temp) const
	{
		auto di = std::filesystem::directory_iterator(temp);
		std::vector<std::filesystem::directory_entry> res;
		for (auto &d : di)
		{
			auto comp = d.path().filename().string().at(0);
			if (comp != '.' || !hide_dot_files)
			{
				res.push_back(d);
			}
		}
		return res;
	}
};

} // namespace ssf
