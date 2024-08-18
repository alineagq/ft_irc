#include "../../include/network/TcpConnection.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h> // Add this line to include the necessary header file for NI_MAXHOST and NI_MAXSERV

TcpConnection::TcpConnection(int socket, const sockaddr_in& clientAddr)
    : clientSocket(socket), clientAddr(clientAddr) {}

void TcpConnection::handleConnection() {
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
    std::cout << " connected on " << ntohs(clientAddr.sin_port) << std::endl;

    std::cout << host << " connected on " << svc << std::endl;

    handleClient();
}

void TcpConnection::handleClient() {
    char buf[4096];
    while (true) {
        memset(buf, 0, sizeof(buf));

        int bytesReceived = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (bytesReceived == -1) {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << std::string(buf, 0, bytesReceived) << std::endl;

        send(clientSocket, buf, bytesReceived, 0);
    }

    ::close(clientSocket);
}