#pragma once

#include <unistd.h>
#include <iostream>
#include <netdb.h>

class User {
    public:
        User();
        User(int &socket, std::string clientIP);
        User(const User &other);
        User &operator=(const User &other);
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

		void setCapNegotiationComplete(bool value);
		bool isCapNegotiationComplete() const;

    private:
		std::string _clientIP;
        int _socketFd;
        std::string _username;
        std::string _nickname;
        bool _authenticated;
        bool _isOperator;
        std::string _buffer;
		bool _capNegotiationComplete;

};
