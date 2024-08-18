#include "network/Socket.hpp"
#include "network/TcpConnection.hpp"
#include <iostream>
#include <cstdlib> // for atoi

int main(int argc, char* argv[]) {
    // Check if the port number is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }

    // Parse the port number
    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number. Port must be between 1 and 65535." << std::endl;
        return -2;
    }

    Socket serverSocket;

    if (!serverSocket.create()) {
        std::cerr << "Can't create a socket!" << std::endl;
        return -3;
    }

    if (!serverSocket.bind(port)) {
        std::cerr << "Can't bind to IP/port " << port << std::endl;
        return -4;
    }

    if (!serverSocket.listen()) {
        std::cerr << "Can't listen!" << std::endl;
        return -5;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientSocket = serverSocket.accept(clientAddr);
        if (clientSocket == -1) {
            std::cerr << "Problem with client connecting!" << std::endl;
            continue;
        }

        TcpConnection connection(clientSocket, clientAddr);
        connection.handleConnection();
    }

    serverSocket.close();
    return 0;
}