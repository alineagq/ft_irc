#include "network/Socket.hpp"
#include "network/TcpConnection.hpp"
#include <iostream>
#include <cstdlib> 
#include "utils/Logger.hpp"

Logger logger("server.log", "server_error.log");

int main(int argc, char* argv[]) {
    if (argc != 2) {
        logger.error("Usage: " + std::string(argv[0]) + " <port>");
        return EINVAL;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        logger.error("Invalid port number. Port must be between 1 and 65535.");
        return -2;
    }

    Socket serverSocket;

    if (!serverSocket.create()) {
        logger.error("Can't create a socket!");
        return -3;
    }

    if (!serverSocket.bind(port)) {
        logger.error("Can't bind to IP/port " + std::to_string(port));
        return -4;
    }

    if (!serverSocket.listen()) {
        logger.error("Can't listen!");
        return -5;
    }

    logger.info("Server listening on port " + std::to_string(port));

    while (true) {
        sockaddr_in clientAddr;
        int clientSocket = serverSocket.accept(clientAddr);
        if (clientSocket == -1) {
            logger.error("Problem with client connecting!");
            continue;
        }

        TcpConnection connection(clientSocket, clientAddr);
        connection.handleConnection();
    }

    serverSocket.close();
    return 0;
}