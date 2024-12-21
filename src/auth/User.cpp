#include "../../include/auth/User.hpp"

User::User() {}
User::User(int &socket): _socketFd(socket) {}
User::~User() {}

User& User::operator=(const User& other) {
    if (this != &other) {
        this->_socketFd = other._socketFd;
        this->_name = other._name;
        this->_nickname = other._nickname;
    }
    return *this;
}

int User::getSocket() {
    return _socketFd;
}

void User::closeSocket() {
    if (_socketFd >= 0) {
        ::close(_socketFd);
        _socketFd = -1;
    }
}
