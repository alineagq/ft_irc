#pragma once

#include <string>
#include <map>

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        bool _inviteOnly;
        bool _topicLocked;
        std::string _key;
        int _userLimit;
        std::map<std::string, bool> _users;
        std::map<int, std::string> _invitedUsers;

    public:
        Channel();
        Channel(const std::string &name);
        Channel(const Channel &ch);
        Channel &operator=(const Channel &ch);
        ~Channel();

        void setName(const std::string &name);
        const std::string &getName() const;

        void addUser(std::string nick, bool isOp);
        void removeUser(std::string nick);
        bool hasUser(std::string nick) const;
        bool isOperator(std::string nick) const;
        void setOperator(std::string nick, bool op);

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

        const std::map<std::string,bool> &getUserMap() const;
        void addInvtedUser(int fd, std::string nick);
        bool isInvited(std::string nick);
        void removeInvitedUser(std::string nick);
};