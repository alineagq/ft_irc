#pragma once

#include <unistd.h>
#include <iostream>
#include <netdb.h>

class User {
    public:
        User();
        User(int &socket);
        ~User();

        void setNickname(const std::string &nick);
        void setUsername(const std::string &user);
        const std::string &getNickname() const;
        const std::string &getUsername() const;

        bool isAuthenticated() const;
        void authenticate();

        bool isOperator() const;
        void setOperator(bool op);

        int getSocket();
        void closeSocket();

        void appendBuffer(const std::string &data);
        std::string &getBufferRef();

    private:
        // char host[NI_MAXHOST];
        int _socketFd;
        std::string _username;
        std::string _nickname;
        bool _authenticated;
        bool _isOperator;
        std::string _buffer;
};
