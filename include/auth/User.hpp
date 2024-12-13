#ifndef USER_HPP
# define USER_HPP

#include <iostream>

class User {
    public:
        User();
        User(int &fd);
        ~User();
        User &operator=(User &other);
    private:
        int _fd;
        std::string _name;
        std::string _nickname;
        std::string _password;
};

#endif

