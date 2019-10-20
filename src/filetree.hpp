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
          	auto di = std::filesystem::directory_iterator(current_path);
          	// Sanitize
          	int dist = std::distance(std::filesystem::begin(di),std::filesystem::end(di));
          	if (selection < dist - 1) {
            		selection++;
	    	}
	}
	void move_left()
	{

		if (current_path.has_parent_path())
		{
			current_path = current_path.parent_path();
			selection = 0;
		}
	}
        void move_right() {
        	try {
          		auto selected = get_directory_entry(selection);
            		if (selected.is_directory()) {
				 auto probePerm =get_directory_list(get_selected_path()); //probe if filesystem throws permission error 
             			 current_path = get_selected_path();
              		selection = 0;
            		} else if (selected.is_regular_file() || selected.is_character_file()) {
              			try {
               		 		move_right_on_file();
              			} catch (std::bad_function_call &e) {
              			}
            		}
        	 } catch (std::filesystem::filesystem_error &e) {
         
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
			auto selected = get_directory_entry(selection);
			if (selected.is_directory() && !std::filesystem::is_empty(selected))
			{
				return get_directory_list(get_selected_path());
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
			// Get Dir Entry should not throw
		}

		return std::vector<std::filesystem::directory_entry>();
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
	int selection = 0;
	std::filesystem::path current_path;

	std::function<void(void)> move_right_on_file;

	std::filesystem::directory_entry get_directory_entry(unsigned sel) const
	{
		auto di = std::filesystem::directory_iterator(current_path);
		if (get_entry_count() > sel)
		{
			return *std::next(di, sel);
		}
		else
		{
			return *di;
		}
	}

	std::vector<std::filesystem::directory_entry> get_directory_list(std::filesystem::path temp) const
	{
		auto di = std::filesystem::directory_iterator(temp);
		std::vector<std::filesystem::directory_entry> res;
		for (auto &d : di)
		{
			res.push_back(d);
		}
		return res;
	}
};

} // namespace ssf
