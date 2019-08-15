#pragma once
#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>

namespace ssf
{

enum class LogLevel
{
    debug,
    warning,
    error
};

class Log
{
public:
    Log()
    {
        logfile.open("logfile.txt", std::ofstream::out | std::ofstream::trunc);
        logfile << "SSF Log: ";
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        logfile << std::ctime(&now)
                << "\n";
    }

    ~Log()
    {
        logfile.close();
    }

    template <typename T>
    Log &operator<<(T const &t)
    {
        logfile << t;
        return *this;
    }

    Log &operator<<(LogLevel l)
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

extern Log log;

} // namespace ssf