#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "../exceptions/LoggerException.hpp"

#define DEBUG 1

class Logger {
public:
    explicit Logger(const std::string& outputFile, const std::string& errorFile);
    ~Logger();

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

private:
    std::ofstream outputLogFile;
    std::ofstream errorLogFile;

    Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    std::string getCurrentTime() const;
};