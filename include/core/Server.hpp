#pragma once

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
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
#include "../handlers/HandleCommands.hpp"

class Server: public IServer {
    public:
        Server();
        Server(Logger& logger, int port);
        ~Server();
        Server &operator=(Server &other);

        void addUser(User& user);
        static void signalHandler(int signum);
        void closeFds();

        Socket& getSocket();
        int& getEpollFd();
        static bool getSignal();
        std::map<int, User> getUsers();
        
        void handleClientData(int clientFd);

    private:
        int _epollFd;
        int _port;
        static bool _Signal;
        Socket _serverSocket;
        std::map<int, User> _users;
        CommandHandler _commandHandler;
};
