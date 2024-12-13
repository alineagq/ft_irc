#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <sys/epoll.h>
#include <cerrno>
#include <fcntl.h>
#include <signal.h>
#include <cstdlib>
#include <string.h>
#include "Socket.hpp"
#include "../auth/User.hpp"
#include "../logger/Logger.hpp"
#include "../../include/network/Socket.hpp"

class Server {
    public:
        Server();
        Server(Logger& logger, int port);
        ~Server();
        Server &operator=(Server &other);

        void handleUser(int clientSocket);
        void signalHandler(int signum);
        // void setSignals();

        Socket& getSocket();
        int getEpollFd() const;

    private:
        int _epollFd;
        int _port;
        static bool _Signal;
        Socket _serverSocket;
        std::vector<User> _users;
        // std::vector<UserHandler> _userHandlers;
        // std::vector<Channel> _channels;
        // std::vector<ChannelHandler> _channelHandlers;
        // std::vector<Message> _messages;
        // std::vector<MessageHandler> _messageHandlers;
        // std::vector<Command> _commands;
        // std::vector<CommandHandler> _commandHandlers;
        // std::vector<ServerHandler> _serverHandlers;
};

#endif