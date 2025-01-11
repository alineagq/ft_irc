#pragma once

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <sys/stat.h>

class TcpConnection {
public:
    TcpConnection(int socket, const sockaddr_in& clientAddr);
    void handleConnection();

protected:
    int clientSocket;
    sockaddr_in clientAddr;
    void handleClient();
};