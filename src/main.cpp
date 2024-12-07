#include <iostream>
#include <cstdlib>
#include <sstream>  // Include for std::ostringstream
#include <vector>
#include <sys/epoll.h>
#include <fcntl.h>
#include "network/Logger.hpp"
#include "network/Socket.hpp"
#include "network/TcpConnection.hpp"


Logger logger("server.log", "server_error.log");

void handleUser(int clientSocket) {
    char buf[4096];
    const std::string logFileName = "log.txt";

    struct stat buffer;
    if (stat(logFileName.c_str(), &buffer) != 0) {
        // Corrected: Using .c_str() to convert std::string to const char*
        std::ofstream createFile(logFileName.c_str()); // This works in C++98
        if (!createFile) {
            std::cerr << "Error creating the file " << logFileName << std::endl;
            return;
        }
    }

    // Corrected: Using .c_str() to convert std::string to const char*
    std::ofstream logFile(logFileName.c_str(), std::ios::app); // This works in C++98
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

        memset(buf, 0, sizeof(buf));

        int bytesReceived = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (bytesReceived == -1) {
            std::cerr << "There was a connection issue" << std::endl;
        }

        if (bytesReceived == 0) {
            std::cout << "The client disconnected" << std::endl;
        }

        std::string receivedMessage(buf, 0, bytesReceived);
        std::cout << "Received: " << receivedMessage << std::endl;
        logFile << "Received: " << receivedMessage << std::endl;

        int bytesSent = send(clientSocket, buf, bytesReceived, 0);
        if (bytesSent == -1) {
            std::cerr << "Error sending message back to client" << std::endl;
        }

        std::cout << "Echoed message back to client" << std::endl;

    logFile.close();
}

bool configureClient(int epollFd, int clientSocket) {
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for clientSocket" << std::endl;
        ::close(clientSocket);
        return false;
    }

    struct epoll_event clientEvent;
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1) {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        ::close(clientSocket);
        return false;
    }

    std::cout << "Client successfully added to epoll" << std::endl;
    return true;
}


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


    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        logger.error("Failed to create epoll file descriptor");
        return -1;
    }

    struct epoll_event ev, events[10];
    ev.events = EPOLLIN;
    ev.data.fd = serverSocket.getSocket();
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket.getSocket(), &ev) == -1) {
        logger.error("Failed to add server socket to epoll");
        return -1;
    }

    while (true) {
            sockaddr_in clientAddr;
            int numEvents = epoll_wait(epollFd, events, 10, -1);
            if (numEvents == -1) {
                logger.error("Failed to wait for events");
                return -1;
            }

            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == serverSocket.getSocket()) {
                    int clientSocket = serverSocket.accept(clientAddr);
                            if (clientSocket == -1) {
                    std::cerr << "Accept failed!" << std::endl;
                    continue;
                }

                // Configurar o cliente com uma função separada
                if (!configureClient(epollFd, clientSocket)) {
                    std::cerr << "Failed to configure client" << std::endl;
                }
                }
                else
                    handleUser(events[i].data.fd);
            }
    };
    serverSocket.close();
    //need to handle open file descriptors and close fds afetr a client disconnects
    return 0;
}