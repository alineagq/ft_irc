#include "../CommandHandler.hpp"

void CommandHandler::cmdCap(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.empty())
    {
        sendMsg(fd, "Usage: CAP <subcommand> [params]\r\n");
        return;
    }

    std::string subcommand = tokens[0];
    for (std::string::size_type i = 0; i < subcommand.size(); i++)
        subcommand[i] = static_cast<char>(std::toupper(subcommand[i]));

    if (subcommand == "LS")
    {
        sendMsg(fd, ":server CAP * LS :multi-prefix sasl\r\n");
    }
    else if (subcommand == "REQ")
    {
        if (tokens.size() < 2)
        {
            sendMsg(fd, "Usage: CAP REQ :<capabilities>\r\n");
            return;
        }
        sendMsg(fd, ":server CAP * ACK :" + tokens[1] + "\r\n");
    }
    else if (subcommand == "END")
	{
		(*m_users)[fd].setCapNegotiationComplete(true);
		(*m_users)[fd].setUsername((m_waitlist)[fd]);
		sendMsg(fd, "CAP END\r\n");
		if (!(m_waitlist)[fd].empty())
		{
			(m_waitlist).erase(fd);
			sendMsg(fd, "Username set.\r\n");
		}
	}
    else
    {
        sendMsg(fd, "Unknown CAP subcommand.\r\n");
    }
}
