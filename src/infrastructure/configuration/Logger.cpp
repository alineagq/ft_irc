#include "Logger.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#define DEBUG 1

Logger::Logger(const std::string& outputFile, const std::string& errorFile)
    : outputLogFile(outputFile.c_str(), std::ios::app),
      errorLogFile(errorFile.c_str(), std::ios::app) {
    if (!outputLogFile.is_open() || !errorLogFile.is_open()) {
        std::cerr << "Unable to open log files!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Logger::~Logger() {
    if (outputLogFile.is_open()) outputLogFile.close();
    if (errorLogFile.is_open()) errorLogFile.close();
}

std::string Logger::getCurrentTime() const {
    std::time_t now = std::time(0);
    std::tm* ltm = std::localtime(&now);

    std::ostringstream oss;
    oss << "["
        << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday << " "
        << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_min << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_sec
        << "]";

    return oss.str();
}

void Logger::info(const std::string& message) {
    outputLogFile << message;
    if (DEBUG) std::cout << message;
}

void Logger::warning(const std::string& message) {
    outputLogFile << message;
    if (DEBUG) std::cout << message;
}

void Logger::error(const std::string& message) {
    errorLogFile << message;
    if (DEBUG) std::cerr << message;
}

void Logger::fatal(const std::string& message) {
    errorLogFile << message;
    if (DEBUG) std::cerr << message;
    exit(EXIT_FAILURE);
}
