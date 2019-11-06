#pragma once

#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h> // for dimensions

namespace ssf
{

// Class to change the Console to Raw Mode
// Ctor enables Raw
// DTor disables Raw
class ConsoleSettings
{
public:
    ConsoleSettings()
    {
        tcgetattr(STDIN_FILENO, &saved_settings); // Save settings to restore later

        auto new_settings = saved_settings;
/*
        // Flags from https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
        new_settings.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        new_settings.c_oflag &= ~(OPOST);
        new_settings.c_cflag |= (CS8);
        new_settings.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        new_settings.c_cc[VMIN] = 1;  // 1 Byte
        new_settings.c_cc[VTIME] = 1; // 100ms

        tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings);*/
    }

    int get_col()
    {
        struct winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size.ws_col;
    }

    int get_row()
    {
        struct winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size.ws_row;
    }

    ~ConsoleSettings()
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_settings); // Restore saved settings
    }

private:
    struct termios saved_settings; // Saved Console Settings
};

} // namespace ssf
