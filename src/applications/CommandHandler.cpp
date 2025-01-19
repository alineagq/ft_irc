#include "CommandHandler.hpp"

CommandHandler::CommandHandler()
: m_users(0), m_channels(0)
{
}

CommandHandler::~CommandHandler()
{
}

void CommandHandler::init(std::map<int, User> *users,
                          std::map<std::string, Channel> *channels,
                          const std::string &password)
{
    m_users = users;
    m_channels = channels;
    m_serverPassword = password;
}

void CommandHandler::processCommand(const std::string &line, int fd)
{
    std::vector<std::string> tokens = split(line, ' ');
    if (tokens.empty())
        return;
    std::string command = tokens[0];
    std::string param;
    if (tokens.size() > 1)
    {
        std::string::size_type pos = line.find(' ');
        if (pos != std::string::npos)
            param = line.substr(pos + 1);
    }
    for (std::string::size_type i = 0; i < command.size(); i++)
        command[i] = static_cast<char>(std::toupper(command[i]));

    // fazer mapeamento de funções
	// implementar CAP LS
	std::cout << "Command: " << command << std::endl;
	if (command == "CAP")
		cmdCap(param, fd);
    else if (command == "PASS")
        cmdPass(param, fd);
    else if (command == "NICK")
        cmdNick(param, fd);
    else if (command == "USER")
        cmdUser(param, fd);
    else if (command == "JOIN") // leak
        cmdJoin(param, fd);
    else if (command == "PRIVMSG")
        cmdPrivMsg(param, fd);
    else if (command == "KICK")
        cmdKick(param, fd);
    else if (command == "INVITE")
        cmdInvite(param, fd);
    else if (command == "TOPIC")
        cmdTopic(param, fd);
    else if (command == "MODE")
        cmdMode(param, fd);
	else if (command == "QUIT")
		cmdQuit(param, fd);
    else
        sendMsg(fd, "Unknown command.\r\n");
}

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
        ch.setTopic(t.substr(1));
        broadcastChannel(channelName, "Channel topic changed to: " + t + "\r\n");
    }
}

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

void CommandHandler::sendMsg(int fd, const std::string &msg)
{
    std::cout << "---> Sending to " << fd << ": " << msg;
    ::send(fd, msg.c_str(), msg.size(), 0);
}

void CommandHandler::broadcastChannel(const std::string &channelName, const std::string &msg)
{
    if (m_channels->find(channelName) == m_channels->end())
        return;
    Channel &ch = (*m_channels)[channelName];
    const std::map<std::string,bool> &channelUserMap = ch.getUserMap();
	const std::map<int, User> &serverUserMap = *m_users;
    std::map<int, User>::const_iterator it = serverUserMap.begin();
    while (it != serverUserMap.end())
    {
		std::string username =  it->second.getNickname();
		if (channelUserMap.find(username) != channelUserMap.end())
        	::send(it->first, msg.c_str(), msg.size(), 0);
        ++it;
    }
}

void CommandHandler::cmdQuit(const std::string &param, int fd) {
	std::vector<std::string> tokens = split(param, ' ');
	if (tokens.size() < 1) {
		sendMsg(fd, "Usage: QUIT [message]\r\n");
		return;
	}
	std::string msg;
	for (std::vector<std::string>::size_type i = 0; i < tokens.size(); i++) {
		msg += tokens[i];
		if (i + 1 < tokens.size())
			msg += " ";
	}
	std::map<std::string, Channel>::iterator it = m_channels->begin();
	std::string userNick = (*m_users)[fd].getNickname();
	while (it != m_channels->end()) {
		if (it->second.hasUser(userNick)) {
			it->second.removeUser(userNick);
			broadcastChannel(it->first, (*m_users)[fd].getNickname() + " left the channel.\r\n");
		}
		++it;
	}
	sendMsg(fd, "Goodbye!\r\n");
	close(fd);
	m_users->erase(fd);
}
