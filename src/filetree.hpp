#pragma once

#include <iostream>
#include <filesystem>
#include <set>
#include <string>
#include <exception>
#include <type_traits>
#include <new>
#include <functional>

#include "log.hpp"

namespace ssf
{
/*
class Selection
{
public:
	Selection();
	Selection(const std::filesystem::directory_entry &de) : selection(de)
	{
	}
	auto get_selection()
	{
		return selection;
	}

private:
	const std::filesystem::directory_entry &selection;
};
*/
class Selection
{
public:
	using T = std::filesystem::directory_entry;
	Selection() {}
	Selection(T const &d)
	{
		new (&storage) T(d);
		valid = true;
	}
	void set_selection(std::filesystem::path const &d)
	{
		new (&storage) T(d);
		valid = true;
	}
	T const get_selection() const
	{
		if (valid)
		{
			return *reinterpret_cast<T const *>(&storage);
		}
		else
		{
			//throw std::nullptr_t();
		}
	}

private:
	std::aligned_storage<sizeof(T), alignof(T)>::type storage;
	bool valid = false;
};

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
		auto it = dl.find(selection.get_selection());
		if (it != dl.begin())
		{
			selection.set_selection(*(it--));
		}
	}
	void move_down()
	{
		//iterate down
		auto dl = get_directory_list(current_path);
		auto it = dl.find(selection.get_selection());
		if (it != dl.end())
		{
			selection.set_selection(*(it++));
		}
	}
	void move_left()
	{

		if (current_path.has_parent_path() &&
			current_path != current_path.root_path())
		{
			selection.set_selection(get_parent_selection());
			current_path = current_path.parent_path();
		}
	}
	void move_right()
	{
		try
		{
			auto selected = selection.get_selection();
			if (selected.is_directory())
			{
				auto probePerm = get_directory_list(get_selected_path()); //probe if filesystem throws permission error
				current_path = get_selected_path();
				selection = *probePerm.begin();
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
		return selection.get_selection().path();
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
		return selection.get_selection();
	}
	auto get_parent_selection() -> std::filesystem::path
	{
		if (current_path.has_parent_path())
		{
			auto parent_dir = get_directory_list(current_path.parent_path());
			auto dir_name = current_path.filename();
			auto it = std::find_if(parent_dir.begin(), parent_dir.end(),
								   [&](std::filesystem::directory_entry const &dir) {
									   return dir.path().filename() == dir_name;
								   });
			if (it != parent_dir.end())
			{
				return *it;
			}
			else
			{
				return *parent_dir.end();
			}
		}
		else
		{
			return current_path;
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
			return std::set<std::filesystem::directory_entry>();
		}
	}

	// TODO: Remove cascaded try/catch and multiple default returns
	auto get_right() const
	{
		try
		{
			if (get_directory_list(current_path).size() != 0)
			{
				auto selected = selection.get_selection();
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

		return std::set<std::filesystem::directory_entry>();
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

	std::size_t get_entry_count() const
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
	std::set<std::filesystem::directory_entry> selection_list;
	Selection selection;

	std::function<void(void)> move_right_on_file;

	std::set<std::filesystem::directory_entry> get_directory_list(std::filesystem::path temp) const
	{
		auto di = std::filesystem::directory_iterator(temp);
		std::set<std::filesystem::directory_entry> res;
		for (auto &d : di)
		{
			auto comp = d.path().filename().string().at(0);
			if (comp != '.' || !hide_dot_files)
			{
				res.insert(d);
			}
		}
		//fix selection
		// is not const
		/*
		if (res.find(selection.get_selection()) == res.end())
		{
			selection.set_selection(*res.rbegin());
		}
		*/
		return res;
	}
};
} // namespace ssf
