#pragma once

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include "../../include/core/IServer.hpp"

class HandleMessages {
    public:
        HandleMessages();
        ~HandleMessages();
        static void handleMessage(int clientSocket);
};