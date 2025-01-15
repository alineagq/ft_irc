#pragma once

#include "../auth/User.hpp"
#include <map>

class IServer {
public:
    virtual ~IServer() {};
    virtual void addUser(User& user) = 0;
    virtual std::map<int, User> getUsers() = 0;
};
