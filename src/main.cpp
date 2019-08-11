#include <iostream>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
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

		struct termios new_settings = saved_settings;

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
constexpr auto clear = "\x1b[2J";
constexpr auto reset_cursor = "\x1b[H";
constexpr auto hide_cursor = "\x1b[?25l";
constexpr auto show_cursor = "\x1b[?25h";
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
			std::cout << "not found";
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

// Build a full page
// Use a string to receive data
class ConsolePageBuilder
{
public:
	ConsolePageBuilder()
	{
	}
	ConsolePageBuilder &add(std::string s)
	{
		stream_buffer << s;
		return *this;
	}
	ConsolePageBuilder &add_line(std::string s)
	{
		stream_buffer << s << "\r\n";
		return *this;
	}
	ConsolePageBuilder &add_default_start()
	{
		add(console_command::hide_cursor);
		add(console_command::reset_cursor);
		add(console_command::clear);
		return *this;
	}
	ConsolePageBuilder &add_default_end()
	{
		add(console_command::show_cursor);
		return *this;
	}
	auto str()
	{
		return stream_buffer.str();
	}

private:
	std::ostringstream stream_buffer;
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

		input_handler.register_callback('q', []() {
			throw close_program_ex_t();
		});

		input_handler.register_callback('w', [&]() {
			//screen.move_cursor(-1, 0);
		});

		input_handler.register_callback('a', [&]() {
			//screen.move_cursor(0, -1);
		});

		input_handler.register_callback('s', [&]() {
			//screen.move_cursor(1, 0);
		});

		input_handler.register_callback('d', [&]() {
			//screen.move_cursor(0, 1);
		});

		ConsolePageBuilder page;

		page.add_default_start()
			.add_line("First Line")
			.add_line("Second")
			.add(console_command::set_cursor(1, 10))
			.add_default_end();

		screen << page.str();

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
