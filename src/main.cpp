#include <iostream>
#include <cstdlib>
#include <sstream>  // Include for std::ostringstream
#include <vector>
#include <sys/epoll.h>
#include <cerrno>
#include <signal.h>
#include "../include/logger/Logger.hpp"
#include "../include/network/Server.hpp"
#include "../include/auth/HandleUser.hpp"

void setSignals();
Logger logger("server.log", "server_error.log");
bool Server::_Signal = false;
 //-> initialize the static boolean

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
        exit(EXIT_FAILURE);
    }

    Server server(logger, port);
    HandleUser handleUser;

    int epollFd = server.getEpollFd();
    int serverFd = server.getSocket().getFd();
    if (serverFd <= 0) {
        logger.error("Invalid server socket file descriptor");
        exit(EXIT_FAILURE);
    }

    try {
        setSignals();
    } catch (std::exception& e) {
        logger.error("Failed to set signals: " + std::string(e.what()));
        exit(EXIT_FAILURE);
    }


    struct epoll_event ev, events[10];
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    // Add the server socket to the epoll
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1) {
        logger.error("Failed to add server socket to epoll: " + std::string(strerror(errno)) + "\n");
        exit(EXIT_FAILURE);
    }
    while (Server::getSignal() == false) {
            sockaddr_in clientAddr;
            // wait for events on the epoll
            int numEvents = epoll_wait(epollFd, events, 10, -1);
            if (numEvents == -1) {
                logger.error("Failed to wait for events");
                return -1;
            }
            //handle the events
            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == serverFd)
                {
                    int clientSocket = server.getSocket().accept(clientAddr);
                    if (clientSocket == -1) {
                        logger.error("Failed to accept client connection: " + std::string(strerror(errno)));
                        continue;
                    }
                    if (!handleUser.configureClient(server, clientSocket, epollFd)) {
                        logger.error("Failed to configure client");
                        continue;
                    }
                }
                else
                    handleUser.handleUser(events[i].data.fd);
            }
    };
    server.closeFds();
    return 0;
}

void setSignals() {
    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
}