#include "network/Socket.hpp"
#include "network/TcpConnection.hpp"
#include <iostream>
#include <cstdlib>
#include "network/Logger.hpp"
#include <sstream>  // Include for std::ostringstream

Logger logger("server.log", "server_error.log");

int main(int argc, char* argv[]) {
    if (argc != 2) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Usage: " << argv[0] << " <port>";
        logger.error(oss.str());
        return EINVAL;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Invalid port number. Port must be between 1 and 65535.";
        logger.error(oss.str());
        return -2;
    }

    Socket serverSocket;

    if (!serverSocket.create()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't create a socket!";
        logger.error(oss.str());
        return -3;
    }

    if (!serverSocket.bind(port)) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't bind to IP/port " << port;
        logger.error(oss.str());
        return -4;
    }

    if (!serverSocket.listen()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't listen!";
        logger.error(oss.str());
        return -5;
    }

    // Convert the success message to string using ostringstream
    std::ostringstream oss;
    oss << "Server listening on port " << port;
    logger.info(oss.str());

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
