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

class DummyLog
{
public:
    DummyLog()
    {
    }

    ~DummyLog()
    {
    }

    template <typename T>
    DummyLog &operator<<(T const &t)
    {
        static_cast<void>(t);
        return *this;
    }

    DummyLog &operator<<(LogLevel l)
    {
        static_cast<void>(l);
        return *this;
    }
};

extern DummyLog log;

} // namespace ssf