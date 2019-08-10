#include <iostream>
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
class ConsoleRawMode{
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
		new_settings.c_cc[VMIN] = 1; // 1 Byte
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

class ConsoleScreen{
	public:
	ConsoleScreen()
	{

	}
	~ConsoleScreen()
	{
		clear(); // Clear screen on exit
	}
	void clear()
	{
		write(STDOUT_FILENO, "\x1b[2J", 4);
		reset_cursor();
	}
	void reset_cursor()
	{
		write(STDOUT_FILENO, "\x1b[H", 3);
	}
	void set_cursor(int x, int y)
	{
		std::ostringstream oss;
		oss << "\x1b[" << x+1 << ";" << y+1 << "H";
		write(STDOUT_FILENO, oss.str().data(), oss.str().size());
	}
	void hide_cursor()
	{
		write(STDOUT_FILENO, "\x1b[?25l", 6);
	}
	void show_cursor()
	{
		write(STDOUT_FILENO, "\x1b[?25h", 6);
	}
	void draw_base()
	{
		// Demo Function
		std::string buffer;
		buffer.append("~TEST");
		for(int i = 0; i < 24; i++){
			buffer.append("~\r\n");
		}
		draw(buffer);
	}
	void draw(std::string const& buffer)
	{
		hide_cursor();
		clear();
		write(STDOUT_FILENO, buffer.data(), buffer.length());
		set_cursor(c_x, c_y);
		show_cursor();
	}
	void move_cursor(int x, int y)
	{
		c_x += x;
		c_x = std::clamp(c_x, 0, size_x);

		c_y += y;
		c_y = std::clamp(c_y, 0, size_y);

		set_cursor(c_x, c_y);
	}
	private:
	int size_x = 20;
	int size_y = 20;
	int c_x = 10;
	int c_y = 10;
};

constexpr char ctrl_key(char const c)
{
	return c & 0x1f;
}

class ConsoleInputHandler{
public:
	void process_key_press()
	{
		auto c = read_key_blocking();
		try{
			callbacks.at(c)(); // Execute callback
		}catch(const std::out_of_range &e){
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
			while(read(STDIN_FILENO, &c, 1) != 1)
				;
			return c;
		}
private:
	std::unordered_map<char,std::function<void(void)>> callbacks;
};

struct close_program_ex_t{}; // used to throw to close the program

int main()
{
	try{
		ConsoleRawMode console_settings; // Enables Raw Mode

		ConsoleInputHandler input_handler;

		ConsoleScreen screen;

		input_handler.register_callback('q', []()
		{
			throw close_program_ex_t();
		});

		input_handler.register_callback('w', [&]()
		{
			screen.move_cursor(-1,0);
		});

		input_handler.register_callback('a', [&]()
		{
			screen.move_cursor(0,-1);
		});

		input_handler.register_callback('s', [&]()
		{
			screen.move_cursor(1,0);
		});

		input_handler.register_callback('d', [&]()
		{
			screen.move_cursor(0,1);
		});

		screen.draw_base();

		while(true){
			input_handler.process_key_press();
		}
	}catch(close_program_ex_t){
		// Makes sure to call all dtors
		return 0;
	}

	return 0;
}
