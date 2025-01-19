#include "../CommandHandler.hpp"

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
	std::string fullMsg = "";
	std::string userNick = (*m_users)[fd].getNickname();
	if ((*m_channels)[target].isOperator(userNick) == true)
    	fullMsg = "[OP]" + userNick + " sends to [" + target + "]: " + msg + "\r\n";
	else
		fullMsg = userNick + " sends to [" + target + "]: "+ msg + "\r\n";
    if (target.size() > 0 && target[0] == '#')
    {
        if (m_channels->find(target) == m_channels->end())
        {
            sendMsg(fd, "No such channel.\r\n");
            return;
        }
        if (!(*m_channels)[target].hasUser(userNick))
        {
            sendMsg(fd, "You're not on that channel.\r\n");
            return;
        }
        broadcastChannel(target, fullMsg);
    }
    else
    {
        bool found = false;
        std::map<int, User>::iterator it = m_users->begin();
        while (it != m_users->end())
        {
            if (it->second.getNickname() == target)
            {
                sendMsg(it->first, fullMsg);
				sendMsg(fd, fullMsg);
                found = true;
                break;
            }
            ++it;
        }
        if (!found)
            sendMsg(fd, "No such nick.\r\n");
    }
}
