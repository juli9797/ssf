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
		update();
	}

	void move_up()
	{
		//iterate up
		if(!selection.empty()){
			auto it = find_element(active_col,*selection.begin());
			if(it != active_col.begin()){
				update_selection(*(--it));	
			}
		}	
		update();
			
	}
	void move_down()
	{
		//iterate down
		if(!selection.empty()){
			auto de = *selection.begin();
			auto it = find_element(active_col,de);
			if(++it != active_col.end()){
				update_selection(*(it));
		       	}
		}
		update();
	}
	void move_left()
	{

		if (current_path.has_parent_path() &&
			current_path != current_path.root_path())
		{
			current_path = current_path.parent_path();
			if(!parent_selection.empty()){
				update_selection(parent_selection.at(0));
			}
		}
		update();
	}
	void move_right()
	{
		try
		{
			if(!selection.empty())
			{
				auto selected = *selection.begin();
				if (selected.is_directory())
				{
					//THIS IS A DIRTY HACK AND SHOULD BE DONE PROPERLY ASAP
					auto probePerm = get_directory_list(selected.path()); //probe if filesystem throws permission error

					current_path = selected.path();
					if(!probePerm.empty()){
						update_selection(*(probePerm.begin()));
					}
				
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
		update();
	}
	//SHOULD BE PRIVATE IN FUTURE! get_selection contains all necessary information!
	//will keep it for now bc main.cpp depends on it -Julian
	auto get_selected_path() const -> std::filesystem::path
	{
		return (*selection.begin()).path();
	}
	
	auto get_current() 
	{
		return active_col;
	}

	auto get_current_path() const
	{
		return current_path;
	}
	auto get_selection() const
	{
		return selection;
	}
	auto get_parent_selection()-> std::vector<std::filesystem::directory_entry>
	{
		return parent_selection;
	}

	//and this
	auto get_left() 
	{
	
		return left_col;
	}

	// TODO: Remove cascaded try/catch and multiple default returns
	auto get_right() 
	{
		return right_col;
	}
	void set_hide_dot_files(bool val)
	{
		hide_dot_files = val;
		update();
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
	
	//DEPRECATED
	/*
	std::size_t get_entry_count() 
	{
		auto dir = active_col;
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
	a}*/
	bool selection_valid(){
	
		return selection.size() == 1;
	}
	void update(){
		//reset
		active_col.clear();
		left_col.clear();
		right_col.clear();
		//active Column
		active_col = get_directory_list(current_path);
		check_selection();
		//left Column
		if (current_path.has_relative_path())
		{
			left_col = get_directory_list(current_path.parent_path());
		}
		check_parent_selection();
		//right Column
		try
		{
			if (active_col.size() != 0 && !selection.empty())
			{
				auto selected = *selection.begin();
				if (selected.is_directory() && !std::filesystem::is_empty(selected))
				{
					right_col = get_directory_list(get_selected_path());
				}
			}
		}
		catch (std::filesystem::filesystem_error &e)
		{
		}		
	}

private:
	//state variables
	bool hide_dot_files = true;
	std::filesystem::path current_path;

	//primary selections
	std::vector<std::filesystem::directory_entry> selection;
	std::vector<std::filesystem::directory_entry> parent_selection;
	
	//secondary selection
	std::vector<std::filesystem::directory_entry> selection_list;

	//colums
	std::vector<std::filesystem::directory_entry> left_col;
	std::vector<std::filesystem::directory_entry> active_col;
	std::vector<std::filesystem::directory_entry> right_col;
	
	std::function<void(void)> move_right_on_file;
	/*
	 *selection helper functions
	 */	
	void check_selection(){
		//check if not valid
		if(!selection.empty()){
			if(find_element(active_col, selection.at(0)) == active_col.end()){
				selection.clear();			
			}
		}
		//default to first entry if possible
		if(selection.empty() && !active_col.empty()){
			update_selection(*active_col.begin());
		}
		
	}
	void check_parent_selection(){
		if (current_path.has_parent_path())
		{
			auto parent_dir = left_col;
			auto dir_name = current_path.filename();
			auto it = std::find_if(parent_dir.begin(), parent_dir.end(),
								   [&](std::filesystem::directory_entry &dir) {
									   return dir.path().filename() == dir_name;
								   });
			if (it != parent_dir.end())
			{
				update_parent_selection(*it);
			}
			else
			{
				//This case should not happen but whooo knows...
				parent_selection.clear();	
			}
		}
		else
		{
			parent_selection.clear();
		}
	}
	void update_selection(std::filesystem::directory_entry de){
		selection.clear();
		selection.emplace_back(de);	
	}
	void update_parent_selection(std::filesystem::directory_entry de){
		parent_selection.clear();
		parent_selection.emplace_back(de);	
	}

	auto find_element(std::vector<std::filesystem::directory_entry> & dl, std::filesystem::directory_entry de) 
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
		return res;
	}
};

} // namespace ssf
