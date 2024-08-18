#include "../../include/network/Socket.hpp"
#include <unistd.h>
#include <cstring>

Socket::Socket() : sock(-1) {}

Socket::~Socket() {
    if (sock != -1) {
        close();
    }
}

bool Socket::create() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    return sock != -1;
}

bool Socket::bind(unsigned short port) {
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;
    return ::bind(sock, (sockaddr*)&hint, sizeof(hint)) != -1;
}

bool Socket::listen(int backlog) {
    return ::listen(sock, backlog) != -1;
}

int Socket::accept(sockaddr_in& clientAddr) {
    socklen_t clientSize = sizeof(clientAddr);
    return ::accept(sock, (sockaddr*)&clientAddr, &clientSize);
}

void Socket::close() {
    ::close(sock);
    sock = -1;
}