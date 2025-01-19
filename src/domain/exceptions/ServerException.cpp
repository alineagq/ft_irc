#include "ServerException.hpp"

ServerException::ServerException(const std::string& message)
    : _msg(message) {}

ServerException::~ServerException() throw() {}

const char* ServerException::what() const throw() {
    return _msg.c_str();
}