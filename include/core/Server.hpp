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
#include <signal.h>
#include "../network/Socket.hpp"
#include "../auth/User.hpp"
#include "../logger/Logger.hpp"
#include "../../include/network/Socket.hpp"
#include "../../include/interfaces/IServer.hpp"
#include "../handlers/HandleCommands.hpp"
#include "../core/Channel.hpp"

class Server: public IServer {
    public:
        Server();
        Server(Logger& logger, int port);
        ~Server();
        Server &operator=(Server &other);
		
		bool run();
		bool configureClient(int clientSocket);
		bool configurePoll(struct epoll_event &ev);
        void handleClientData(int clientFd);
        
        void addUser(User& user);
        std::map<int, User> getUsers();

        Socket& getSocket();
        int& getEpollFd();
        bool getSignal();

		void setSignals();
		static void signalHandler(int signum);

        void closeFds();

    private:
        int _epollFd;
        int _port;
        static bool _isRunning;
        Socket _serverSocket;
        std::map<int, User> _users;
        CommandHandler _commandHandler;
        std::map<std::string, Channel> _channels;
};
