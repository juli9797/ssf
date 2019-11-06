#pragma once

#include <iostream>
#include <filesystem>
#include <set>
#include <string>
#include <exception>
#include <functional>

#include "log.hpp"

namespace ssf
{


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
		//iterate up
		auto dl = get_directory_list(current_path);
		if(selection_valid()){
			auto it = find_element(dl,*selection.begin());
			if(it != dl.begin()){
				update_selection(*(--it));	
			}
		std::cout << (*selection.begin()).path();
		}	
			
	}
	void move_down()
	{
		//iterate down
		auto dl = get_directory_list(current_path);
		if(selection_valid()){
			auto de = *selection.begin();
			auto it = find_element(dl,de);
			if(++it != dl.end()){
				update_selection(*(it));
		       	}
		}
	}
	void move_left()
	{

		if (current_path.has_parent_path() &&
			current_path != current_path.root_path())
		{
			auto de = get_parent_selection();
			update_selection(de);
		}
	}
	void move_right()
	{
		try
		{
			if(selection_valid())
			{
				auto selected = *selection.begin();
				if (selected.is_directory())
				{
					auto probePerm = get_directory_list(get_selected_path()); //probe if filesystem throws permission error
					current_path = get_selected_path();
					update_selection(*probePerm.begin());
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
		}
		catch (std::filesystem::filesystem_error &e)
		{
		}
	}

	auto get_selected_path() const -> std::filesystem::path
	{
		return (*selection.begin()).path();
	}
	//mod this
	auto get_current() 
	{
		return get_directory_list(current_path);
	}

	auto get_current_path() const
	{
		return current_path;
	}
	auto get_selection() const
	{
		return (*selection.begin());
	}
	auto get_parent_selection() -> std::filesystem::directory_entry
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
				return (*it);
			}
			else
			{
				return (*parent_dir.end());
			}
		}
		else
		{
			std::filesystem::directory_entry de = {};
			return de;
		}
	}

	//and this
	auto get_left() 
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
	auto get_right() 
	{
		try
		{
			if (get_directory_list(current_path).size() != 0)
			{
				auto selected = *selection.begin();
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
		auto dl = get_directory_list(current_path); 
		
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

	std::size_t get_entry_count() 
	{
		auto dir = std::filesystem::directory_iterator(current_path);
		if (hide_dot_files)
		{

			return std::count_if(std::filesystem::begin(dir),
								 std::filesystem::end(dir),
								 [](auto const &d) {
									 return d.path().filename().string().at(0) != '.';
								 });
		}
		else
		{
			return std::distance(std::filesystem::begin(dir),
								 std::filesystem::end(dir));
		}
	}

private:
	bool hide_dot_files = true;
	std::filesystem::path current_path;
	std::vector<std::filesystem::directory_entry> selection_list;
	std::vector<std::filesystem::directory_entry> selection;
	
	
	std::function<void(void)> move_right_on_file;
	
	bool selection_valid(){
	
		return selection.size() == 1;
	}
	void update_selection(std::filesystem::directory_entry de){
		selection.clear();
		selection.emplace_back(de);	
	}
	auto find_element(std::vector<std::filesystem::directory_entry> dl, std::filesystem::directory_entry de) 
		-> std::vector<std::filesystem::directory_entry>::iterator
	{
		auto it = std::find(dl.begin(), dl.end(), de);
		return it;
	}
	std::vector<std::filesystem::directory_entry> get_directory_list(std::filesystem::path temp)
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
		//fix selection
		//if selection not valid and dir not empty, set selection to first element
		if(!selection_valid() && !res.empty()){
			update_selection(res.at(0));
		}
		/*if (find_element(res,selection[0]) == res.end() )
		{
			update_selection(*res.rbegin());
		}*/
		return res;
	}
};

} // namespace ssf
