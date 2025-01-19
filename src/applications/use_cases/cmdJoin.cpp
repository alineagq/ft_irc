#include "../CommandHandler.hpp"

void CommandHandler::cmdJoin(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: JOIN <#channel>\r\n");
        return;
    }
    std::string channelName = tokens[0];
	std::string userNick = (*m_users)[fd].getNickname();
	std::string username = (*m_users)[fd].getUsername();

	if (userNick.empty())
	{
		sendMsg(fd, "You must set a nickname first.\r\n");
		return;
	}
	if (username.empty())
	{
		sendMsg(fd, "You must set a username first.\t\n");
		return;
	}
    if (channelName.size() < 1 || channelName[0] != '#')
    {
        sendMsg(fd, "Channel name must start with '#'.\r\n");
        return;
    }
    if (m_channels->find(channelName) == m_channels->end())
    {
		(*m_channels)[channelName] = Channel(channelName);
    	(*m_channels)[channelName].addUser(userNick, true);
	}
	else if ((*m_channels)[channelName].hasUser(userNick))
	{
		sendMsg(fd, "Already in channel: " + channelName + "\r\n");
		return;
	}
	else if ((*m_channels)[channelName].getInviteOnly() && !(*m_channels)[channelName].isInvited((*m_users)[fd].getNickname()))
	{
		sendMsg(fd, "You're not invited to that channel.\r\n");
		return;
	}
	else if ((*m_channels)[channelName].getUserLimit() != -1 && (int)(*m_channels)[channelName].getUserMap().size() >= (*m_channels)[channelName].getUserLimit())
	{
		sendMsg(fd, "Channel is full.\r\n");
		return;
	}
	else
    {
		if (!(*m_channels)[channelName].getKey().empty())
		{
			std::string key = (*m_channels)[channelName].getKey();
			if (tokens.size() < 2 || tokens[1] != key)
			{
				sendMsg(fd, "Wrong channel key.\r\n");
				return;
			}
			else
				(*m_channels)[channelName].addUser(userNick, false);
				
		}
		else
			(*m_channels)[channelName].addUser(userNick, false);
	}


	std::string topic = "Channel Topic: " + (*m_channels)[channelName].getTopic();
    sendMsg(fd, "Joined channel " + channelName + "\n" + topic + "\r\n");
    broadcastChannel(channelName, userNick + " joined " + channelName + "\r\n");
}
