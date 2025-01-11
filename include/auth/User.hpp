#pragma once

#include <unistd.h>
#include <iostream>
#include <netdb.h>

class User {
    public:
        User();
        User(int &socket);
        ~User();
        User &operator=(const User &other);

        void closeSocket();

        int getSocket();
    private:
        // char host[NI_MAXHOST];
        int _socketFd;
        std::string _name;
        std::string _nickname;
};
