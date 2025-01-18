#pragma once

#include "../../include//auth/User.hpp"
#include <map>

class IServer {
public:
    virtual ~IServer() {};
    virtual bool run() = 0;
};
