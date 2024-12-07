#include "TcpConnection.hpp"

TcpConnection::TcpConnection(int socket, const sockaddr_in& clientAddr)
    : clientSocket(socket), clientAddr(clientAddr) {}

void TcpConnection::handleConnection() {
    char host[NI_MAXHOST];
    inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
    std::cout << "Client " << host << " connected on port " << ntohs(clientAddr.sin_port) << std::endl;
}
