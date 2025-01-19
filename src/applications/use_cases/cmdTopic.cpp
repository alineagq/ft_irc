#include "../CommandHandler.hpp"

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
	std::string executorNick = (*m_users)[fd].getNickname();
    if (!ch.hasUser(executorNick))
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
        if (ch.getTopicLocked() && !ch.isOperator(executorNick))
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
        std::string topic = t.substr(1);
        ch.setTopic(topic);
        broadcastChannel(channelName, "Channel topic changed to: " + topic + "\r\n");
    }
}
