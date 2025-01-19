#include "Logger.hpp"

Logger::Logger(const std::string& outputFile, const std::string& errorFile)
    : outputLogFile(outputFile.c_str(), std::ios::app),
      errorLogFile(errorFile.c_str(), std::ios::app) {
    if (!outputLogFile.is_open() || !errorLogFile.is_open()) 
        throw LoggerException("Unable to open log files!");
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
    std::ostringstream oss;
    oss << getCurrentTime() << " [INFO] " << message << std::endl;
    outputLogFile << oss.str();
    if (DEBUG) std::cout << oss.str();
}

void Logger::warning(const std::string& message) {
    std::ostringstream oss;
    oss << getCurrentTime() << " [WARNING] " << message << std::endl;
    outputLogFile << oss.str();
    if (DEBUG) std::cout << oss.str();
}

void Logger::error(const std::string& message) {
    std::ostringstream oss;
    oss << getCurrentTime() << " [ERROR] " << message << std::endl;
    errorLogFile << oss.str();
    if (DEBUG) std::cerr << oss.str();
}

void Logger::fatal(const std::string& message) {
    std::ostringstream oss;
    oss << getCurrentTime() << " [FATAL] " << message << std::endl;
    errorLogFile << oss.str();
    if (DEBUG) std::cerr << oss.str();
    throw LoggerException(oss.str());
}
