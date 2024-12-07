#include "../../include/auth/User.hpp"

User::User() {}
User::User(int &fd) : _fd(fd) {}
User::~User() {}