#pragma once

#include <vector>
#include <string>
#include "../auth/User.hpp"
#include "Channel.hpp"

class CommandHandler
{
private:
    std::map<int, User> *m_users;
    std::map<std::string, Channel> *m_channels;
    std::string m_serverPassword;

public:
    CommandHandler();
    ~CommandHandler();

    void init(std::vector<User> *users,
              std::map<std::string, Channel> *channels,
              const std::string &password);

    void processCommand(const std::string &line, int fd);

private:
    void cmdPass(const std::string &param, int fd);
    void cmdNick(const std::string &param, int fd);
    void cmdUser(const std::string &param, int fd);
    void cmdJoin(const std::string &param, int fd);
    void cmdPrivMsg(const std::string &param, int fd);
    void cmdKick(const std::string &param, int fd);
    void cmdInvite(const std::string &param, int fd);
    void cmdTopic(const std::string &param, int fd);
    void cmdMode(const std::string &param, int fd);

    void sendMsg(int fd, const std::string &msg);
    void broadcastChannel(const std::string &channelName, const std::string &msg);
};