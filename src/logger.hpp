#pragma once
#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>

enum class LogLevel
{
    debug,
    warning,
    error
};

class Logger
{
public:
    Logger()
    {
        logfile.open("logfile.txt", std::ofstream::out | std::ofstream::trunc);
        logfile << "SSF Log: ";
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        logfile << std::ctime(&now)
                << "\n";
    }

    ~Logger()
    {
        logfile.close();
    }

    template <typename T>
    Logger &operator<<(T const &t)
    {
        logfile << t;
        return *this;
    }

    Logger &operator<<(LogLevel l)
    {
        switch (l)
        {
        case LogLevel::debug:
            logfile << "> ";
            break;
        case LogLevel::warning:
            logfile << ">! ";
            break;
        case LogLevel::error:
            logfile << "!!! ";
            break;
        };
        return *this;
    }

private:
    std::ofstream logfile;
};

extern Logger logger;