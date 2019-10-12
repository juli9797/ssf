#include <cstdlib> // For system call, move to system call abstraction later
#include <unistd.h>

#include "console_settings.hpp"
#include "console_screen.hpp"
#include "console_input_handler.hpp"
#include "console_page.hpp"

#include "filetree.hpp"

#include "log.hpp"
#include "close_programm_exception.hpp"

void draw_to_page(ssf::ConsolePage &page, ssf::Filetree &tree)
{
	page.clear_entries();
	page.add_col(tree.get_left());
	page.add_col(tree.get_current());
	page.add_col(tree.get_right());

	page.set_selection(tree.get_selection());
}

int main()
{
	try
	{
		ssf::log << "Start\n";

		ssf::ConsoleSettings console_settings; // Enables Raw Mode

		auto cols = console_settings.get_col();
		auto rows = console_settings.get_row();

		ssf::ConsoleInputHandler input_handler;

		ssf::ConsoleScreen screen;

		ssf::ConsolePage page;
		page.set_col_width((cols / 3) - 6)
			.set_spacing(2)
			.set_row_count(rows - 10);

		ssf::Filetree tree;

		page.add_col(tree.get_left());
		page.add_col(tree.get_current());
		page.add_col(tree.get_right());

		screen << page.str();

		input_handler.register_callback('q', []() {
			throw ssf::close_program_ex_t();
		});

		input_handler.register_callback('j', [&]() {
			tree.move_down();
			draw_to_page(page, tree);
			screen << page.str();
		});

		input_handler.register_callback('k', [&]() {
			tree.move_up();
			draw_to_page(page, tree);
			screen << page.str();
		});

		input_handler.register_callback('h', [&]() {
			tree.move_left();
			draw_to_page(page, tree);
			screen << page.str();
		});

		input_handler.register_callback('l', [&]() {
			tree.move_right();
			draw_to_page(page, tree);
			screen << page.str();
		});

		// Test Callback to open a shell
		// cmd and call logic needs to be implemented elsewhere
		input_handler.register_callback('s', [&]() {
			std::string cmd = {"gnome-terminal --working-directory="};
			cmd += tree.get_current_path();
			cmd += " &> /dev/null";
			// maybe try fork execl instead might be faster
			int ret = system(cmd.c_str());
			static_cast<void>(ret);
		});
		//Quick and dirty xdg-open
		input_handler.register_callback('o', [&]() {
			std::string cmd = "xdg-open ";
			cmd += tree.get_current_path() / tree.get_current()[tree.get_selection()];
			cmd += " &> /dev/null";
			ssf::log << cmd << "\n";
			// maybe try fork execl instead might be faster
			int ret = system(cmd.c_str());
			static_cast<void>(ret);
		});
		//Quick and dirty vim edit
		input_handler.register_callback('e', [&]() {
			std::string cmd = "vim ";
			cmd += tree.get_current_path() / tree.get_current()[tree.get_selection()];
			ssf::log << cmd << "\n";
			// maybe try fork execl instead might be faster
			int ret = system(cmd.c_str());
			static_cast<void>(ret);
		});

		// Command input
		input_handler.register_callback(':', [&]() {
			screen << page.str();
		});

		ssf::log << "Main keypress loop\n";

		while (true)
		{
			input_handler.process_key_press();
		}
	}
	catch (ssf::close_program_ex_t)
	{
		// Makes sure to call all dtors
		return 0;
	}

	return 0;
}
