#pragma once

#include "../entities/User.hpp"
#include <map>

class IServer {
public:
    virtual ~IServer() {};
    virtual bool run() = 0;
};
