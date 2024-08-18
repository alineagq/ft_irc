#pragma once

#include <netinet/in.h>

class TcpConnection {
public:
    TcpConnection(int socket, const sockaddr_in& clientAddr);
    void handleConnection();

protected:
    int clientSocket;
    sockaddr_in clientAddr;
    void handleClient();
};