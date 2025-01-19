#include "../../include/handlers/HandleCommands.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>

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

static std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str.c_str());
    std::string tmp;
    while (std::getline(iss, tmp, delim))
    {
        if (!tmp.empty())
            tokens.push_back(tmp);
    }
    return tokens;
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


	std::cout << "Joining channel" << std::endl;
	std::string topic = "Channel Topic: " + (*m_channels)[channelName].getTopic();
    sendMsg(fd, "Joined channel " + channelName + "\n" + topic + "\r\n");
    broadcastChannel(channelName, userNick + " joined " + channelName + "\r\n");
}

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
    	fullMsg = "[OP]" + userNick + " <" + target + ">: " + msg + "\r\n";
	else
		fullMsg = userNick + " <" + target + ">: "+ msg + "\r\n";
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
		std::cout << "PRIV MSG: "<< fullMsg.c_str() << std::endl;
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
                found = true;
                break;
            }
            ++it;
        }
        if (!found)
            sendMsg(fd, "No such nick.\r\n");
    }
}

void CommandHandler::cmdKick(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: KICK <#channel> <nick>\r\n");
        return;
    }
    std::string channelName;
    std::string victimNick;
	std::string message;
	for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i][0] == '#')
        {
            channelName = tokens[i];
        }
        else if (tokens[i][0] == ':')
        {
            if (victimNick.empty())
            {
                victimNick = tokens[i].substr(1); // Remover o ":"
            }
        }
    }
	size_t pos = param.find(":");
	if (pos != std::string::npos)
	{
		pos = param.find(":", pos + 1);
		if (pos != std::string::npos)
			message = param.substr(pos + 1);
	}
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
	std::string executorNick = (*m_users)[fd].getNickname();
    if (!ch.hasUser(executorNick) || !ch.isOperator(executorNick))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    int victimFd = -1;
    std::map<int, User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->second.getNickname() == victimNick && ch.hasUser(victimNick))
        {
            victimFd = it->first;
            break;
        }
        ++it;
    }
    if (victimFd == -1)
    {
        sendMsg(fd, "User not found in channel.\r\n");
        return;
    }
	if (ch.getInviteOnly())
		ch.removeInvitedUser(victimNick);
    ch.removeUser(victimNick);
    sendMsg(victimFd, "You were KICKED from " + channelName + ":" + message + "\r\n");
    broadcastChannel(channelName, victimNick + " was kicked from channel.\r\n");
}

void CommandHandler::cmdInvite(const std::string &param, int fd)
{
    std::vector<std::string> tokens = split(param, ' ');
    if (tokens.size() < 2)
    {
        sendMsg(fd, "Usage: INVITE <nick> <#channel>\r\n");
        return;
    }
    std::string nick = tokens[0];
    std::string channelName = tokens[1];
    if (m_channels->find(channelName) == m_channels->end())
    {
        sendMsg(fd, "Channel doesn't exist.\r\n");
        return;
    }
    Channel &ch = (*m_channels)[channelName];
	std::string executorNick = (*m_users)[fd].getNickname();
    if (!ch.hasUser(executorNick) || !ch.isOperator(executorNick))
    {
        sendMsg(fd, "You're not channel operator.\r\n");
        return;
    }
    int invitedFd = -1;
    std::map<int, User>::iterator it = m_users->begin();
    while (it != m_users->end())
    {
        if (it->second.getNickname() == nick)
        {
            invitedFd = it->first;
            break;
        }
        ++it;
    }
    if (invitedFd == -1)
    {
        sendMsg(fd, "No such user.\r\n");
        return;
    }
    sendMsg(invitedFd, "You've been invited to " + channelName + "\r\n");
    broadcastChannel(channelName, nick + " was invited to the channel.\r\n");
	ch.addInvtedUser(invitedFd, nick);
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
            if (tokens.size() > 2)
            {
                int limit = add ? std::atoi(tokens[2].c_str()) : -1;
                ch.setUserLimit(limit);
            }
        }
        i++;
    }
    broadcastChannel(channelName, "Channel mode updated.\r\n");
}

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
		(m_waitlist).erase(fd);
		sendMsg(fd, "CAP END\r\n");
		sendMsg(fd, "Username set.\r\n");
	}
    else
    {
        sendMsg(fd, "Unknown CAP subcommand.\r\n");
    }
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
