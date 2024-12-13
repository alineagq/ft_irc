#include "../../include/network/Server.hpp"

bool    Server::_Signal = false; //-> initialize the static boolean

Server::Server(Logger& logger, int port): _epollFd(epoll_create1(0)), _port(port) {
    if (_epollFd == -1) {
        logger.error("Failed to create epoll file descriptor");
        exit(-1);
    }

     if (!_serverSocket.create()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't create a socket!";
        logger.error(oss.str());
        throw std::runtime_error("Can't create a socket!");
    }

    if (!_serverSocket.bind(port)) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't bind to IP/port " << port;
        logger.error(oss.str());
        throw std::runtime_error("Can't bind to IP/port");
    }

    if (!_serverSocket.listen()) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't listen!";
        logger.error(oss.str());
        throw std::runtime_error("Can't listen!");
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

// void Server::setSignals() {
//     signal(SIGINT, Server::signalHandler); //catch the signal (ctrl + c)
//     signal(SIGQUIT, Server::signalHandler); // catch the signal (ctrl + \)
// }

Socket& Server::getSocket() {
    return _serverSocket;
}

int Server::getEpollFd() const {
    return _epollFd;
}
