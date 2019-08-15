#include "console_raw_mode.hpp"
#include "console_screen.hpp"
#include "console_input_handler.hpp"
#include "console_page.hpp"

#include "filetree.hpp"

#include "log.hpp"
#include "close_programm_exception.hpp"

int main()
{
	try
	{
		ssf::log << "Start\n";

		ssf::ConsoleRawMode console_settings; // Enables Raw Mode

		ssf::ConsoleInputHandler input_handler;

		ssf::ConsoleScreen screen;

		ssf::ConsolePage page(35);

		ssf::Filetree tree;

		page.add_col(tree.get_left());
		page.add_col(tree.get_current());
		page.add_col(tree.get_right());

		screen << page.str();

		input_handler.register_callback('q', []() {
			throw ssf::close_program_ex_t();
		});

		input_handler.register_callback('j', [&]() {
			page.selection_incr();
			page.clear_entries();

			tree.move_down();
			page.add_col(tree.get_left());
			page.add_col(tree.get_current());
			page.add_col(tree.get_right());

			screen << page.str();
		});

		input_handler.register_callback('k', [&]() {
			page.selection_decr();
			page.clear_entries();

			tree.move_up();
			page.add_col(tree.get_left());
			page.add_col(tree.get_current());
			page.add_col(tree.get_right());

			screen << page.str();
		});

		input_handler.register_callback('h', [&]() {
			page.reset_selection();
			page.clear_entries();

			tree.move_left();
			page.add_col(tree.get_left());
			page.add_col(tree.get_current());
			//page.add_col(tree.get_right());

			screen << page.str();
		});

		input_handler.register_callback('l', [&]() {
			page.reset_selection();
			page.clear_entries();

			tree.move_right();
			page.add_col(tree.get_left());
			page.add_col(tree.get_current());
			//page.add_col(tree.get_right());

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
