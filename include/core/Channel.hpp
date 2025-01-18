#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include "../interfaces/IUser.hpp"

class Channel
{
private:
    std::string _name;
    std::string _topic;
    bool _inviteOnly;
    bool _topicLocked;
    std::string _key;
    int _userLimit;
    std::map<int, bool> _users;
	std::map<int, std::string> _invitedUsers;
public:
    Channel();
    Channel(const std::string &name);
    ~Channel();

    void setName(const std::string &name);
    const std::string &getName() const;

    void addUser(int fd, bool isOp);
    void removeUser(int fd);
    bool hasUser(int fd) const;
    bool isOperator(int fd) const;
    void setOperator(int fd, bool op);

    void setTopic(const std::string &topic);
    const std::string &getTopic() const;

    void setInviteOnly(bool inviteOnly);
    bool getInviteOnly() const;

    void setTopicLocked(bool locked);
    bool getTopicLocked() const;

    void setKey(const std::string &key);
    const std::string &getKey() const;

    void setUserLimit(int limit);
    int getUserLimit() const;

    const std::map<int,bool> &getUserMap() const;
	void addInvtedUser(int fd, std::string nick);
	bool isInvited(std::string nick);
	void removeInvitedUser(std::string nick);
};

#endif
