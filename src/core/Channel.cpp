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

void Channel::addUser(int fd, bool isOp)
{
    _users[fd] = isOp;
}

void Channel::removeUser(int fd)
{
    _users.erase(fd);
}

bool Channel::hasUser(int fd) const
{
    return (_users.find(fd) != _users.end());
}

bool Channel::isOperator(int fd) const
{
    std::map<int,bool>::const_iterator it = _users.find(fd);
    if (it != _users.end())
        return it->second;
    return false;
}

void Channel::setOperator(int fd, bool op)
{
    std::map<int,bool>::iterator it = _users.find(fd);
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

void Channel::setKey(const std::string &key)
{
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

const std::map<int,bool> &Channel::getUserMap() const
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