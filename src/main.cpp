#include <iostream>
#include <cstdlib>
#include <sstream>  // Include for std::ostringstream
#include <vector>
#include <sys/epoll.h>
#include <cerrno>
#include "../include/logger/Logger.hpp"
#include "../include/network/Server.hpp"
#include "../include/auth/HandleUser.hpp"

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

    Server server(logger, port);
    HandleUser handleUser;

    int epollFd = server.getEpollFd();

    int serverFd = server.getSocket().getFd();
    if (serverFd <= 0) {
        logger.error("Invalid server socket file descriptor");
        exit(-1);
    }

    std::cout << "serverFd: "<< serverFd << std::endl;

    struct epoll_event ev, events[10];
    ev.events = EPOLLIN;
    ev.data.fd = serverFd; // o erro e por conta que não consigo adicionar o servidor a lista do epoll
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1) {
        logger.error("Failed to add server socket to epoll: " + std::string(strerror(errno)) + "\n");
        exit(-1);
    }


    while (true) {
            sockaddr_in clientAddr;
            int numEvents = epoll_wait(epollFd, events, 10, -1);
            if (numEvents == -1) {
                logger.error("Failed to wait for events");
                return -1;
            }

            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == serverFd) {
                    int clientSocket = server.getSocket().accept(clientAddr);
                            if (clientSocket == -1) {
                        std::cerr << "Accept failed!" << std::endl;
                        continue;
                    }
                    if (!handleUser.configureClient(epollFd, clientSocket)) {
                        std::cerr << "Failed to configure client" << std::endl;
                        continue;
                     }
                }
                else
                    handleUser.handleUser(events[i].data.fd);
            }
    };
    //need to handle open file descriptors and close fds after a client disconnects
    return 0;
}