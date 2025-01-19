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

