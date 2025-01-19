#include "../CommandHandler.hpp"

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
	std::string executorNick = (*m_users)[fd].getNickname();
    if (!ch.hasUser(executorNick) || !ch.isOperator(executorNick))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    int invitedFd = -1;
    std::map<int, User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->second.getNickname() == nick)
        {
            invitedFd = it->first;
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
	ch.addInvtedUser(invitedFd, nick);
}
