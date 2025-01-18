#include "../../include/core/Channel.hpp"

Channel::Channel()
: _inviteOnly(false), _topicLocked(false), _userLimit(-1)
{
}

Channel::Channel(const std::string &name)
: _name(name), _inviteOnly(false), _topicLocked(false), _userLimit(-1)
{
}

Channel::~Channel()
{
}

void Channel::setName(const std::string &name)
{
    _name = name;
}

const std::string &Channel::getName() const
{
    return _name;
}

void Channel::addUser(std::string nick, bool isOp) //change
{
    _users[nick] = isOp;
}

void Channel::removeUser(std::string nick) //change
{
    _users.erase(nick);
}

bool Channel::hasUser(std::string nick) const //change
{
    return (_users.find(nick) != _users.end());
}

bool Channel::isOperator(std::string nick) const //change
{
    std::map<std::string,bool>::const_iterator it = _users.find(nick);
    if (it != _users.end())
        return it->second;
    return false;
}

void Channel::setOperator(std::string nick, bool op) //change
{
    std::map<std::string, bool>::iterator it = _users.find(nick);
    if (it != _users.end())
        it->second = op;
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

const std::string &Channel::getTopic() const
{
    return _topic;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    _inviteOnly = inviteOnly;
}

bool Channel::getInviteOnly() const
{
    return _inviteOnly;
}

void Channel::setTopicLocked(bool locked)
{
    _topicLocked = locked;
}

bool Channel::getTopicLocked() const
{
    return _topicLocked;
}

#include <iostream>

void Channel::setKey(const std::string &key)
{
	std::cout << "KEY: " << key << std::endl;
    _key = key;
}

const std::string &Channel::getKey() const
{
    return _key;
}

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
}

int Channel::getUserLimit() const
{
    return _userLimit;
}

const std::map<std::string,bool> &Channel::getUserMap() const
{
    return _users;
}

void Channel::addInvtedUser(int fd, std::string nick)
{
	_invitedUsers[fd] = nick;
}

bool Channel::isInvited(std::string nick)
{
	std::map<int, std::string>::iterator it = _invitedUsers.begin();
	while (it != _invitedUsers.end())
	{
		if (it->second == nick)
			return true;
		++it;
	}
	return false;
}

void Channel::removeInvitedUser(std::string nick)
{
	std::map<int, std::string>::iterator it = _invitedUsers.begin();
	while (it != _invitedUsers.end())
	{
		if (it->second == nick)
		{
			_invitedUsers.erase(it);
			break;
		}
		++it;
	}
}