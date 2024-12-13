#ifndef HANDLEUSER_HPP
# define HANDLEUSER_HPP

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>

class HandleUser {
    public:
        HandleUser();
        ~HandleUser();
        HandleUser &operator=(HandleUser &other);

        void handleUser(int clientSocket);
        bool configureClient(int epollFd, int clientSocket);
    private:
        int _serverSocket;

};

#endif