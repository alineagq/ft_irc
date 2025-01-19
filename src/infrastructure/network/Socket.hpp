#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket {
public:
    Socket();
    ~Socket();
    bool create();
    bool bind(unsigned short port);
    bool listen(int backlog = SOMAXCONN);
    int accept(sockaddr_in& clientAddr);
    int setSocketLinger();
    int getFd() const;
    void close();

private:
    int sock;

    Socket(const Socket&);
    Socket& operator=(const Socket&);
};
