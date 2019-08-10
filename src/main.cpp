#include <iostream>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <exception>

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
	void draw_base()
	{
		clear();
		for(int i = 0; i < 24; i++){
			write(STDOUT_FILENO, "~\r\n", 3);
		}
		reset_cursor();
	}
	private:
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

struct close_program_t{}; // used to throw to close the program

int main()
{
	try{
		ConsoleRawMode console_settings; // Enables Raw Mode

		ConsoleInputHandler input_handler;

		ConsoleScreen screen;

		input_handler.register_callback('q', []()
		{
			throw close_program_t();
		});

		screen.draw_base();

		while(true){
			input_handler.process_key_press();
		}
	}catch(close_program_t){
		// Makes sure to call all dtors
		return 0;
	}

	return 0;
}
