#pragma once

#include "../auth/User.hpp"
#include <vector>

class IServer {
public:
    virtual ~IServer() {};
    virtual void addUser(const User& user) = 0;
    virtual std::vector<User> getUsers() = 0;
};