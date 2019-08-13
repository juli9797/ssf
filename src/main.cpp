#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <functional>
#include <exception>
#include <algorithm>

#include <unistd.h>
#include <termios.h>

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

namespace console_command
{
constexpr auto new_line = "\r\n";
//constexpr auto clear = "\x1b[2J"; // doesnt really clear
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
} // namespace console_command

class ConsoleScreen
{
public:
	ConsoleScreen()
	{
	}
	~ConsoleScreen()
	{
		*this << console_command::clear
			  << console_command::reset_cursor; // Clear screen on exit
	}
	ConsoleScreen &operator<<(std::string const buffer)
	{
		write(STDOUT_FILENO, buffer.data(), buffer.length());
		return *this;
	}

private:
	int size_x = 20;
	int size_y = 100;
	int c_x = 10;
	int c_y = 10;
};

constexpr char ctrl_key(char const c)
{
	return c & 0x1f;
}

class ConsoleInputHandler
{
public:
	void process_key_press()
	{
		auto c = read_key_blocking();
		try
		{
			callbacks.at(c)(); // Execute callback
		}
		catch (const std::out_of_range &e)
		{
			// Debug Out
		}
	}

	void register_callback(char c, std::function<void(void)> callable)
	{
		callbacks[c] = callable;
	}

protected:
	auto read_key_blocking() -> char
	{
		char c;
		while (read(STDIN_FILENO, &c, 1) != 1)
			;
		return c;
	}

private:
	std::unordered_map<char, std::function<void(void)>> callbacks;
};

class ConsolePage
{
public:
	ConsolePage(int space = 15) : spacing(space)
	{
	}
	auto str()
	{
		std::ostringstream p;
		p << console_command::default_start;
		int col_counter = 0;
		for (auto &col : entries)
		{
			int index = 0;
			for (auto &s : col)
			{
				p << console_command::set_cursor(index, col_counter * spacing);
				if (index == selection && col_counter == active_col)
				{
					p << console_command::bold_enable;
					p << s;
					p << console_command::bold_disable;
				}
				else
				{
					p << s;
				}

				index++;
			}

			col_counter++;
		}
		p << console_command::hide_cursor;
		return p.str();
	}
	void add_col(std::vector<std::string> c)
	{
		entries.push_back(c);
	}
	void selection_incr()
	{
		if (static_cast<int>(entries.size()) <= active_col)
		{
			selection = 0;
		}
		if (selection < static_cast<int>(entries.at(active_col).size()) - 1)
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
	int selection = 0;
	int active_col = 1;
	const int spacing;
	std::vector<std::vector<std::string>> entries;
};

struct close_program_ex_t
{
}; // used to throw to close the program

int main()
{
	try
	{
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
