#include "console_raw_mode.hpp"
#include "console_screen.hpp"
#include "console_input_handler.hpp"
#include "console_page.hpp"

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

		ssf::ConsolePage page;
		page.add_col({"first", "second", "third"});
		page.add_col({"third", "fourth", "third", "third", "third"});
		page.add_col({"fifth", "sixth", "third", "fifth", "sixth", "third"});

		screen << page.str();

		input_handler.register_callback('q', []() {
			throw ssf::close_program_ex_t();
		});

		input_handler.register_callback('j', [&]() {
			page.selection_incr();
			screen << page.str();
		});

		input_handler.register_callback('k', [&]() {
			page.selection_decr();
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
