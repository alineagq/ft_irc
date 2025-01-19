#include "User.hpp"

User::User() {}

User::User(int &socket, std::string clientIP): _socketFd(socket), _authenticated(false), _isOperator(false), _capNegotiationComplete(false) {
	_clientIP = clientIP;
}

User::User(const User &other)
{
    _socketFd = other._socketFd;
    _username = other._username;
    _nickname = other._nickname;
    _authenticated = other._authenticated;
    _isOperator = other._isOperator;
    _buffer = other._buffer;
    _capNegotiationComplete = other._capNegotiationComplete;
}

User &User::operator=(const User &other)
{
    if (this != &other)
    {
        _socketFd = other._socketFd;
        _username = other._username;
        _nickname = other._nickname;
        _authenticated = other._authenticated;
        _isOperator = other._isOperator;
        _buffer = other._buffer;
        _capNegotiationComplete = other._capNegotiationComplete;
    }
    return *this;
}

User::~User() {}

void User::setNickname(const std::string &nick)
{
    _nickname = nick;
}

void User::setUsername(const std::string &user)
{
    _username = user;
}

const std::string &User::getNickname() const
{
    return _nickname;
}

const std::string &User::getUsername() const
{
    return _username;
}


bool User::isAuthenticated() const
{
    return _authenticated;
}

void User::authenticate()
{
    _authenticated = true;
}

bool User::isOperator() const
{
    return _isOperator;
}

void User::setOperator(bool op)
{
    _isOperator = op;
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

void User::appendBuffer(const std::string &data)
{
    _buffer += data;
}

std::string &User::getBufferRef()
{
    return _buffer;
}

void User::setCapNegotiationComplete(bool value) {
	_capNegotiationComplete = value;
}

bool User::isCapNegotiationComplete() const {
	return _capNegotiationComplete;
}
