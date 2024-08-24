#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>

class Logger {
public:

    Logger(const std::string& outputFile, const std::string& errorFile);
    ~Logger();

    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

private:
    std::ofstream outputLogFile;
    std::ofstream errorLogFile;

    std::string getCurrentTime() const;

};

#endif
