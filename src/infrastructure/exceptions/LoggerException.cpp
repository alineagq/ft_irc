#include "LoggerException.hpp"

LoggerException::LoggerException(const std::string& message)
    : _msg(message) {}

LoggerException::~LoggerException() throw() {}

const char* LoggerException::what() const throw() {
    return _msg.c_str();
}