#include "../../include/core/Server.hpp"

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

    if (_serverSocket.setSocketLinger() == -1) {
        // Convert the error message to string using ostringstream
        std::ostringstream oss;
        oss << "Can't set socket linger!";
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }

    int serverSocketFd = _serverSocket.getFd();
    // setSignals();
    // Convert the success message to string using ostringstream
     if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for clientSocket" << std::endl;
        ::close(serverSocketFd);
        exit(EXIT_FAILURE);
    }
    std::ostringstream oss;
    oss << "Server listening on port " << port << std::endl;
    logger.info(oss.str());

}

Server::~Server() {
    close(_epollFd);
    _serverSocket.close();
}

void Server::handleClientData(int clientFd)
{
    char buf[512];
    std::memset(buf, 0, sizeof(buf));

    int recvBytes = recv(clientFd, buf, 511, 0);
    if (recvBytes <= 0)
    {
        return;
    }
    _users[clientFd].appendBuffer(std::string(buf));
    std::string &bufferRef = _users[clientFd].getBufferRef();
    std::string::size_type pos = bufferRef.find("\\r\\n");
    std::cout << "buff ref: " << bufferRef << std::endl;
    std::cout << "pos: " << pos << std::endl;
    while (pos != std::string::npos)
    {
        std::string line = bufferRef.substr(0, pos);
        bufferRef.erase(0, pos + 2);
        std::cout << "Client command: " << line << std::endl;
        _commandHandler.processCommand(line, clientFd);
        pos = bufferRef.find("\\r\\n");
    }
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

int& Server::getEpollFd() {
    return _epollFd;
}

bool Server::getSignal() {
    return _Signal;
}

void Server::addUser(User& user) {
    _users[user.getSocket()] = user;
}

void Server::closeFds() {
    for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); it++) {
        if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL) == -1)
        {
            // oss << "Failed to remove client socket from epoll: " << strerror(errno) << std::endl;
            std::cerr << "Failed to remove client socket from epoll: " << strerror(errno) << std::endl;
        }
        it->second.closeSocket();
        _users.erase(it);
    }   
}

std::map<int, User> Server::getUsers() {
    return _users;
}