#include "../CommandHandler.hpp"

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