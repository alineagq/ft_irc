#include "CommandHandler.hpp"

void CommandHandler::cmdMode(const std::string &param, int fd)
{
	// adicioinar logica de setar um operador caso o ultimo operador saia do canal
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: MODE <#channel> <modes> [args]\r\n");
        return;
    }
    std::string channelName = tokens[0];
	std::cout << "Channel name: " << channelName << std::endl;
    std::string modes       = tokens[1];
	std::cout << "Modes: " << modes << std::endl;
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
	std::string executorNick = (*m_users)[fd].getNickname();
    if (!ch.isOperator(executorNick))
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
            if (tokens.size() > 2 && add)
            {
				ch.setKey(tokens[2]);
            }
			else if (!add)
			{
				ch.setKey("");
			}
        }
        else if (c == 'o')
        {
			std::string promotedUser = tokens.back();
            ch.setOperator(promotedUser, add);
        }
        else if (c == 'l')
        {
			if (!add)
				ch.setUserLimit(-1);
            else if (tokens.size() > 2)
            {
                int limit = add ? std::atoi(tokens[2].c_str()) : -1;
				std::cout << "Limit: " << limit << std::endl;
                ch.setUserLimit(limit);
            }
        }
        i++;
    }
    broadcastChannel(channelName, "Channel mode updated.\r\n");
}
