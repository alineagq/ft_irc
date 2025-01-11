#include "../../include/handlers/HandleCommands.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>

CommandHandler::CommandHandler()
: m_users(0), m_channels(0)
{
}

CommandHandler::~CommandHandler()
{
}

void CommandHandler::init(std::vector<User> *users,
                          std::map<std::string, Channel> *channels,
                          const std::string &password)
{
    m_users = users;
    m_channels = channels;
    m_serverPassword = password;
}

static std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str.c_str());
    std::string tmp;
    while (std::getline(iss, tmp, delim))
    {
        if (!tmp.empty())
            tokens.push_back(tmp);
    }
    return tokens;
}

void CommandHandler::processCommand(const std::string &line, int fd)
{
    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.empty())
        return;

    std::string command = tokens[0];
    std::string param;
    if (tokens.size() > 1)
    {
        std::string::size_type pos = line.find(' ');
        if (pos != std::string::npos)
            param = line.substr(pos + 1);
    }
    for (std::string::size_type i = 0; i < command.size(); i++)
        command[i] = static_cast<char>(std::toupper(command[i]));

    if (command == "PASS")
        cmdPass(param, fd);
    else if (command == "NICK")
        cmdNick(param, fd);
    else if (command == "USER")
        cmdUser(param, fd);
    else if (command == "JOIN")
        cmdJoin(param, fd);
    else if (command == "PRIVMSG")
        cmdPrivMsg(param, fd);
    else if (command == "KICK")
        cmdKick(param, fd);
    else if (command == "INVITE")
        cmdInvite(param, fd);
    else if (command == "TOPIC")
        cmdTopic(param, fd);
    else if (command == "MODE")
        cmdMode(param, fd);
    else
        sendMsg(fd, "Unknown command.\r\n");
}

void CommandHandler::cmdPass(const std::string &param, int fd)
{
    if (!(*m_users)[fd].isAuthenticated())
    {
        if (param == m_serverPassword)
        {
            sendMsg(fd, "Password ok.\r\n");
            (*m_users)[fd].authenticate();
        }
        else
        {
            sendMsg(fd, "Wrong password.\r\n");
        }
    }
    else
    {
        sendMsg(fd, "PASS already sent.\r\n");
    }
}

void CommandHandler::cmdNick(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: NICK <nick>\r\n");
        return;
    }
    (*m_users)[fd].setNickname(tokens[0]);
    sendMsg(fd, "Nickname set.\r\n");
}

void CommandHandler::cmdUser(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: USER <username>\r\n");
        return;
    }
    (*m_users)[fd].setUsername(tokens[0]);
    sendMsg(fd, "Username set.\r\n");
}

void CommandHandler::cmdJoin(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: JOIN <#channel>\r\n");
        return;
    }
    std::string channelName = tokens[0];
    if (channelName.size() < 1 || channelName[0] != '#')
    {
        sendMsg(fd, "Channel name must start with '#'.\r\n");
        return;
    }
    if (m_channels->find(channelName) == m_channels->end())
        (*m_channels)[channelName] = Channel(channelName);

    (*m_channels)[channelName].addUser(fd, false);
    sendMsg(fd, "Joined channel " + channelName + "\r\n");
    broadcastChannel(channelName, (*m_users)[fd].getNickname() + " joined " + channelName + "\r\n");
}

void CommandHandler::cmdPrivMsg(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: PRIVMSG <#channel|nick> <message>\r\n");
        return;
    }
    std::string target = tokens[0];
    std::string msg;
    std::vector<std::string>::size_type i = 1;
    while (i < tokens.size())
    {
        msg += tokens[i];
        if (i + 1 < tokens.size())
            msg += " ";
        i++;
    }
    std::string fullMsg = (*m_users)[fd].getNickname() + ": " + msg + "\r\n";
    if (target.size() > 0 && target[0] == '#')
    {
        if (m_channels->find(target) == m_channels->end())
        {
            sendMsg(fd, "No such channel.\r\n");
            return;
        }
        if (!(*m_channels)[target].hasUser(fd))
        {
            sendMsg(fd, "You're not on that channel.\r\n");
            return;
        }
        broadcastChannel(target, fullMsg);
    }
    else
    {
        bool found = false;
        std::vector<User>::iterator it = m_users->begin();
        while (it != m_users->end())
        {
            if (it->getNickname() == target)
            {
                sendMsg(it->getSocket(), fullMsg);
                found = true;
                break;
            }
            ++it;
        }
        if (!found)
            sendMsg(fd, "No such nick.\r\n");
    }
}

void CommandHandler::cmdKick(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: KICK <#channel> <nick>\r\n");
        return;
    }
    std::string channelName = tokens[0];
    std::string nick = tokens[1];
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
    if (!ch.hasUser(fd) || !ch.isOperator(fd))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    int victimFd = -1;
    std::vector<User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->getNickname() == nick && ch.hasUser(it->getSocket()))
        {
            victimFd = it->getSocket();
            break;
        }
        ++it;
    }
    if (victimFd == -1)
    {
        sendMsg(fd, "User not found in channel.\r\n");
        return;
    }
    ch.removeUser(victimFd);
    sendMsg(victimFd, "You were KICKed from " + channelName + "\r\n");
    broadcastChannel(channelName, nick + " was kicked from channel.\r\n");
}

void CommandHandler::cmdInvite(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: INVITE <nick> <#channel>\r\n");
        return;
    }
    std::string nick = tokens[0];
    std::string channelName = tokens[1];
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
    if (!ch.hasUser(fd) || !ch.isOperator(fd))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    int invitedFd = -1;
    std::vector<User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->getNickname() == nick)
        {
            invitedFd = it->getSocket();
            break;
        }
        ++it;
    }
    if (invitedFd == -1)
    {
        sendMsg(fd, "No such user.\r\n");
        return;
    }
    sendMsg(invitedFd, "You've been invited to " + channelName + "\r\n");
    broadcastChannel(channelName, nick + " was invited to the channel.\r\n");
}

void CommandHandler::cmdTopic(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: TOPIC <#channel> [new topic]\r\n");
        return;
    }
    std::string channelName = tokens[0];
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
    if (!ch.hasUser(fd))
    {
        sendMsg(fd, "You're not on that channel.\r\n");
        return;
    }
    if (tokens.size() == 1)
    {
        sendMsg(fd, "Channel topic: " + ch.getTopic() + "\r\n");
    }
    else
    {
        if (ch.getTopicLocked() && !ch.isOperator(fd))
        {
            sendMsg(fd, "Only operators can change the topic.\r\n");
            return;
        }
        std::string t;
        for (std::vector<std::string>::size_type i = 1; i < tokens.size(); i++)
        {
            if (i > 1)
                t += " ";
            t += tokens[i];
        }
        ch.setTopic(t);
        broadcastChannel(channelName, "Channel topic changed to: " + t + "\r\n");
    }
}

void CommandHandler::cmdMode(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: MODE <#channel> <modes> [args]\r\n");
        return;
    }
    std::string channelName = tokens[0];
    std::string modes       = tokens[1];
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
    if (!ch.isOperator(fd))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    bool add = true;
    std::string::size_type i = 0;
    while (i < modes.size())
    {
        char c = modes[i];
        if (c == '+')
        {
            add = true;
        }
        else if (c == '-')
        {
            add = false;
        }
        else if (c == 'i')
        {
            ch.setInviteOnly(add);
        }
        else if (c == 't')
        {
            ch.setTopicLocked(add);
        }
        else if (c == 'k')
        {
            if (tokens.size() > 2)
            {
                if (add)
                    ch.setKey(tokens[2]);
                else
                    ch.setKey("");
            }
        }
        else if (c == 'o')
        {
            ch.setOperator(fd, add);
            (*m_users)[fd].setOperator(add);
        }
        else if (c == 'l')
        {
            if (tokens.size() > 2)
            {
                int limit = add ? std::atoi(tokens[2].c_str()) : -1;
                ch.setUserLimit(limit);
            }
        }
        i++;
    }
    broadcastChannel(channelName, "Channel mode updated.\r\n");
}

void CommandHandler::sendMsg(int fd, const std::string &msg)
{
    ::send(fd, msg.c_str(), msg.size(), 0);
}

void CommandHandler::broadcastChannel(const std::string &channelName, const std::string &msg)
{
    if (m_channels->find(channelName) == m_channels->end())
        return;
    Channel &ch = (*m_channels)[channelName];
    const std::map<int,bool> &userMap = ch.getUserMap();
    std::map<int,bool>::const_iterator it = userMap.begin();
    while (it != userMap.end())
    {
        ::send(it->first, msg.c_str(), msg.size(), 0);
        ++it;
    }
}
