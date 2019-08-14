#pragma once
#include <iostream>
#include <fstream>

#include <chrono>

class Logger
{
public:
    Logger()
    {
        logfile.open("logfile.txt", std::ofstream::out | std::ofstream::trunc);
        logfile << "SSF Log "
                << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
                << "\n";
    }

    ~Logger()
    {
        logfile.close();
    }

    template <typename T>
    Logger &operator<<(T const &t)
    {
        logfile << "> " << t;
        return *this;
    }

private:
    std::ofstream logfile;
};

extern Logger logger;