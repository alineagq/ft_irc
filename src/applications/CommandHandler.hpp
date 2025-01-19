#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include "../domain/entities/User.hpp"
#include "../domain/entities/Channel.hpp"
#include "../entrypoint/utils/utils.hpp"
#include "../infrastructure/configuration/Logger.hpp"

class CommandHandler
{
    private:
        std::map<int, User> *m_users;
        std::map<std::string, Channel> *m_channels;
        std::string m_serverPassword;
        std::map<int, std::string> m_waitlist;

        typedef void (CommandHandler::*CommandFunc)(const std::string&, int);
        std::map<std::string, CommandFunc> m_commandMap;

        CommandHandler(const CommandHandler &ch);
        CommandHandler &operator=(const CommandHandler &ch);

        void cmdPass(const std::string &param, int fd);
        void cmdNick(const std::string &param, int fd);
        void cmdUser(const std::string &param, int fd);
        void cmdJoin(const std::string &param, int fd);
        void cmdPrivMsg(const std::string &param, int fd);
        void cmdKick(const std::string &param, int fd);
        void cmdInvite(const std::string &param, int fd);
        void cmdTopic(const std::string &param, int fd);
        void cmdMode(const std::string &param, int fd);
        void cmdCap(const std::string &param, int fd);
        void cmdQuit(const std::string &param, int fd);

        void sendMsg(int fd, const std::string &msg);
        void broadcastChannel(const std::string &channelName, const std::string &msg);
    public:
        CommandHandler();
        ~CommandHandler();

        void init(std::map<int, User> *users,
                  std::map<std::string, Channel> *channels,
                  const std::string &password);
        void processCommand(const std::string &line, int fd);
};
