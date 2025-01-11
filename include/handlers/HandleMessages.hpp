#pragma once

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>

class HandleMessages {
    public:
        HandleMessages();
        ~HandleMessages();
        static void handleMessage(int clientSocket);
};