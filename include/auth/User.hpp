#ifndef USER_HPP
# define USER_HPP

class User {
    public:
        User();
        User(int &fd);
        ~User();
        User &operator=(User &&other);
    private:
        int _fd;
};

#endif

