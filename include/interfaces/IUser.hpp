#pragma once

#include <string>

class IUser {
    public:
        virtual void setNickname(const std::string &nick);
        virtual void setUsername(const std::string &user);
        virtual const std::string &getNickname() const;
        virtual const std::string &getUsername() const;

        virtual bool isAuthenticated() const;
        virtual void authenticate();

        virtual bool isOperator() const;
        virtual void setOperator(bool op);

        virtual int getSocket();
        virtual void closeSocket();
    private:
        int _socketFd;
        std::string _username;
        std::string _nickname;
        bool _authenticated;
        bool _isOperator;
};