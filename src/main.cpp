#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <functional>
#include <exception>
#include <algorithm>

#include <unistd.h>
#include <iostream>
#include <termios.h>

#include "logger.hpp"

// Class to change the Console to Raw Mode
// Ctor enables Raw
// DTor disables Raw
class ConsoleRawMode
{
public:
	ConsoleRawMode()
	{
		tcgetattr(STDIN_FILENO, &saved_settings); // Save settings to restore later

		auto new_settings = saved_settings;

		// Flags from https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
		new_settings.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		new_settings.c_oflag &= ~(OPOST);
		new_settings.c_cflag |= (CS8);
		new_settings.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
		new_settings.c_cc[VMIN] = 1;  // 1 Byte
		new_settings.c_cc[VTIME] = 1; // 100ms

		tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings);
	}
	~ConsoleRawMode()
	{
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_settings); // Restore saved settings
	}

private:
	struct termios saved_settings; // Saved Console Settings
};

namespace c_cmd
{
constexpr auto new_line = "\r\n";
constexpr auto weak_clear = "\x1b[2J"; // doesnt really clear
constexpr auto clear = "\033c";
constexpr auto reset_cursor = "\x1b[H";
constexpr auto hide_cursor = "\x1b[?25l";
constexpr auto show_cursor = "\x1b[?25h";
constexpr auto bold_enable = "\033[1m";
constexpr auto bold_disable = "\033[0m";
constexpr auto default_start = "\x1b[?25l\x1b[H\033c";
auto set_cursor(int x, int y)
{
	std::ostringstream oss;
	oss << "\x1b[" << x + 1 << ";" << y + 1 << "H";
	return oss.str();
}
} // namespace c_cmd

class ConsoleScreen
{
public:
	ConsoleScreen()
	{
		*this << c_cmd::hide_cursor;
	}
	~ConsoleScreen()
	{
		*this << c_cmd::clear
			  << c_cmd::reset_cursor; // Clear screen on exit
	}
	ConsoleScreen const &operator<<(std::string const buffer) const
	{
		std::cout << buffer;
		return *this;
	}

private:
};

constexpr char ctrl_key(char const c)
{
	return c & 0x1f;
}

class ConsoleInputHandler
{
public:
	using callback_t = std::function<void(void)>;

	void process_key_press() const
	{
		auto c = read_key_blocking();
		try
		{
			callbacks.at(c)(); // Execute callback
		}
		catch (const std::out_of_range &e)
		{
			logger << LogLevel::warning << "key " << c << " not registered\n";
		}
	}

	void register_callback(char c, callback_t callable)
	{
		callbacks[c] = callable;
	}

protected:
	auto read_key_blocking() const -> char
	{
		return std::cin.get();
	}

private:
	std::unordered_map<char, callback_t> callbacks;
};

class ConsolePage
{
public:
	ConsolePage(int space = 15) : spacing(space)
	{
	}

	auto str() const
	{
		std::ostringstream p;
		p << c_cmd::clear;

		for (auto col_index = 0u; col_index < entries.size(); col_index++)
		{
			auto col = entries.at(col_index);
			for (auto index = 0u; index < col.size(); index++)
			{
				p << c_cmd::set_cursor(index, col_index * spacing);
				if (index == selection && col_index == active_col)
				{
					p << c_cmd::bold_enable << col.at(index) << c_cmd::bold_disable;
				}
				else
				{
					p << col.at(index);
				}
			}
		}
		p << c_cmd::hide_cursor;
		return p.str();
	}

	void add_col(std::vector<std::string> c)
	{
		entries.push_back(c);
	}

	void selection_incr()
	{
		if (entries.size() <= active_col)
		{
			selection = 0;
		}
		if (selection < entries.at(active_col).size() - 1)
		{
			selection++;
		}
	}

	void selection_decr()
	{
		if (selection > 0)
		{
			selection--;
		}
	}

private:
	unsigned selection = 0u;
	unsigned active_col = 1;
	const int spacing;
	std::vector<std::vector<std::string>> entries;
};

// used to throw to close the program
struct close_program_ex_t
{
};

// Exception from the no globals rule bc log
// Only needs to be instantiated in main.cpp
Logger logger;

int main()
{
	try
	{
		logger << "Started\n";

		ConsoleRawMode console_settings; // Enables Raw Mode

		ConsoleInputHandler input_handler;

		ConsoleScreen screen;

		ConsolePage page;
		page.add_col({"first", "second", "third"});
		page.add_col({"third", "fourth", "third", "third", "third"});
		page.add_col({"fifth", "sixth", "third", "fifth", "sixth", "third"});

		screen << page.str();

		input_handler.register_callback('q', []() {
			throw close_program_ex_t();
		});

		input_handler.register_callback('j', [&]() {
			page.selection_incr();
			screen << page.str();
		});

		input_handler.register_callback('k', [&]() {
			page.selection_decr();
			screen << page.str();
		});

		logger << "Main keypress loop\n";

		while (true)
		{
			input_handler.process_key_press();
		}
	}
	catch (close_program_ex_t)
	{
		// Makes sure to call all dtors
		return 0;
	}

	return 0;
}
