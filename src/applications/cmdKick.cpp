#include "CommandHandler.hpp"

void CommandHandler::cmdKick(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: KICK <#channel> <nick>\r\n");
        return;
    }
    std::string channelName;
    std::string victimNick;
	std::string message;
	for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i][0] == '#')
        {
            channelName = tokens[i];
        }
        else if (tokens[i][0] == ':')
        {
            if (victimNick.empty())
            {
                victimNick = tokens[i].substr(1); // Remover o ":"
            }
        }
    }
	size_t pos = param.find(":");
	if (pos != std::string::npos)
	{
		pos = param.find(":", pos + 1);
		if (pos != std::string::npos)
			message = param.substr(pos + 1);
	}
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
    int victimFd = -1;
    std::map<int, User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->second.getNickname() == victimNick && ch.hasUser(victimNick))
        {
            victimFd = it->first;
            break;
        }
        ++it;
    }
    if (victimFd == -1)
    {
        sendMsg(fd, "User not found in channel.\r\n");
        return;
    }
	if (ch.getInviteOnly())
		ch.removeInvitedUser(victimNick);
    ch.removeUser(victimNick);
    sendMsg(victimFd, "You were KICKED from " + channelName + ":" + message + "\r\n");
    broadcastChannel(channelName, victimNick + " was kicked from channel.\r\n");
}
