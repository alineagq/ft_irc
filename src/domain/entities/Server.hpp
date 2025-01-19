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
#include "User.hpp"
#include "../entities/Channel.hpp"
#include "../interfaces/IServer.hpp"
#include "../exceptions/ServerException.hpp"
#include "../../applications/CommandHandler.hpp" 
#include "../../infrastructure/configuration/Logger.hpp"
#include "../../infrastructure/network/Socket.hpp"

class Server: public IServer {
    public:
        Server(Logger& logger, int port, std::string pw);
        ~Server();
		
		bool run();
		bool configureClient();
		bool configurePoll(struct epoll_event &ev);
        void handleClientData(int clientFd);
        
        void addUser(User& user);
        std::map<int, User> getUsers();

        Socket& getSocket();
        int& getEpollFd();
        bool getSignal();

		void setSignals();
		static void signalHandler(int signum);

		std::string getClientIP(sockaddr_in &clientAddr) const;
        void closeFds();

    private:
        int _epollFd;
        int _port;
		std::string _password;
        static bool _isRunning;
        std::map<int, User> _users;
        std::map<std::string, Channel> _channels;
        CommandHandler _commandHandler;
        Socket _serverSocket;

        Server();
        Server(Server &other);
        Server &operator=(Server &other);
};
