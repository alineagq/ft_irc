#include "../../include/network/TcpConnection.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <sys/stat.h>

TcpConnection::TcpConnection(int socket, const sockaddr_in& clientAddr)
    : clientSocket(socket), clientAddr(clientAddr) {}

void TcpConnection::handleConnection() {
    char host[NI_MAXHOST];
    inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
    std::cout << "Client " << host << " connected on port " << ntohs(clientAddr.sin_port) << std::endl;

    handleClient();
}

void TcpConnection::handleClient() {
    char buf[4096];
    const std::string logFileName = "log.txt";

    // Verifica se o arquivo existe, se não, cria-o
    struct stat buffer;
    if (stat(logFileName.c_str(), &buffer) != 0) {
        std::ofstream createFile(logFileName);
        if (!createFile) {
            std::cerr << "Erro ao criar o arquivo " << logFileName << std::endl;
            return;
        }
    }

    std::ofstream logFile(logFileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

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

        std::string receivedMessage(buf, 0, bytesReceived);
        std::cout << "Received: " << receivedMessage << std::endl;
        logFile << "Received: " << receivedMessage << std::endl;

        int bytesSent = send(clientSocket, buf, bytesReceived, 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending message back to client" << std::endl;
            break;
        }

        std::cout << "Echoed message back to client" << std::endl;
    }

    logFile.close();
    ::close(clientSocket);
}