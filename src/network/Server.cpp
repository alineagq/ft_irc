#include "../../include/network/Server.hpp"

Server::Server(Logger& logger, int port): _epollFd(epoll_create1(0)), _port(port) {
    if (_epollFd == -1) {
        logger.error("Failed to create epoll file descriptor");
        exit(EXIT_FAILURE);
    }

     if (!_serverSocket.create()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't create a socket!";
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }

    if (!_serverSocket.bind(port)) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't bind to IP/port " << port;
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }

    if (!_serverSocket.listen()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't listen!";
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }


    // setSignals();
    // Convert the success message to string using ostringstream
    fcntl(_serverSocket.getFd(), F_SETFL, O_NONBLOCK);
    std::ostringstream oss;
    oss << "Server listening on port " << port << std::endl;
    logger.info(oss.str());

}

Server::~Server() {
    close(_epollFd);
    _serverSocket.close();
}

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	_Signal = true;
}

Socket& Server::getSocket() {
    return _serverSocket;
}

int Server::getEpollFd() const {
    return _epollFd;
}

bool Server::getSignal() {
    return _Signal;
}