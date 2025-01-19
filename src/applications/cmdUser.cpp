#include "CommandHandler.hpp"

void CommandHandler::cmdUser(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
	if (!(*m_users)[fd].isCapNegotiationComplete())
    {
        sendMsg(fd, "Finish CAP negotiation first.\r\n");
		(m_waitlist)[fd] = tokens[0];
        return;
    }
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: USER <username>\r\n");
        return;
    }
    (*m_users)[fd].setUsername(tokens[0]);
    sendMsg(fd, "Username set.\r\n");
}
