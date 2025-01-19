#include "../CommandHandler.hpp"

void CommandHandler::cmdNick(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: NICK <nick>\r\n");
        return;
    }

	std::map<int, User>::iterator it = m_users->begin();
	while (it != m_users->end())
	{
		if (it->second.getNickname() == tokens[0])
		{
			sendMsg(fd, ":irc.araio.com 433 * nickname :Nickname is already in use.\r\n");
			return;
		}
		++it;
	}

    (*m_users)[fd].setNickname(tokens[0]);
    sendMsg(fd, "Nickname set.\r\n");
}
