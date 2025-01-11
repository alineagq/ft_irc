#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include "../../include/interfaces/IServer.hpp"

class HandleUser {
    public:
        HandleUser();
        ~HandleUser();
        HandleUser &operator=(HandleUser &other);

        bool configureClient(IServer& Server, int clientSocket, int epollFd);
    private:
        int _serverSocket;

};
