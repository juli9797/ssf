#include <iostream>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

#include <unistd.h>
#include <termios.h>

void enable_raw()
{
	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);

	raw.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

class ConsoleScreen{
	public:
	private:

};

int main()
{
	std::cout << "start";

	enable_raw();
	
	char c;
	while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
		;
	
	return 0;
}
