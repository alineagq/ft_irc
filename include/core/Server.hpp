#pragma once

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "../network/Socket.hpp"
#include "../auth/User.hpp"
#include "../logger/Logger.hpp"
#include "../../include/network/Socket.hpp"
#include "../../include/interfaces/IServer.hpp"

class Server: public IServer {
    public:
        Server();
        Server(Logger& logger, int port);
        ~Server();
        Server &operator=(Server &other);

        void addUser(const User& user);
        static void signalHandler(int signum);
        void closeFds();

        Socket& getSocket();
        int& getEpollFd();
        static bool getSignal();
        std::vector<User> getUsers();

    private:
        int _epollFd;
        int _port;
        static bool _Signal;
        Socket _serverSocket;
        std::vector<User> _users;
};
