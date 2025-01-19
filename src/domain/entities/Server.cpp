#include "Server.hpp"

extern Logger logger;

Server::Server(Logger& logger, int port, std::string pw):
							_epollFd(epoll_create1(0)), _port(port), _password(pw) {
    if (_epollFd == -1)
        throw ServerException("Failed to create epoll file descriptor");
    
    if (!_serverSocket.create())
        throw ServerException("Failed to create a socket");

    if (!_serverSocket.bind(port))
        throw ServerException("Failed to bind the socket");

    if (!_serverSocket.listen())
        throw ServerException("Failed to listen on the socket");

    if (_serverSocket.setSocketLinger() == -1)
        throw ServerException("Failed to set socket linger");

    int serverSocketFd = _serverSocket.getFd();
    if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) == -1) {
        ::close(serverSocketFd);
        throw ServerException("Failed to set non-blocking mode for server socket");
    }

    std::ostringstream oss;
    oss << "---> Server listening on port " << port;
    logger.info(oss.str());

    _commandHandler.init(&_users, &_channels, _password);
	_isRunning = false;
}

bool Server::_isRunning;

Server::~Server() {
	closeFds();
    close(_epollFd);
    _serverSocket.close();
}

bool Server::run() {
	setSignals();
	struct epoll_event ev, events[512];
	if (!configurePoll(ev))
		return false;

	int serverFd = _serverSocket.getFd();
	while (_isRunning == false)
    {
        int numEvents = epoll_wait(_epollFd, events, 512, -1);
        if (numEvents == -1)
            return false;
        for (int i = 0; i < numEvents; i++) {
            if (events[i].data.fd == serverFd)
            {
                if (!configureClient()) {
                    logger.error("Failed to configure client");
                    continue;
                }
            }
            else
                handleClientData(events[i].data.fd);
        }
    };
	return true;
}

bool Server::configureClient() {
	sockaddr_in clientAddr;
	int clientSocket = _serverSocket.accept(clientAddr);
	if (clientSocket == -1)
        logger.error("Failed to accept client connection");
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        logger.error("Failed to set non-blocking mode for clientSocket");
        ::close(clientSocket);
        return false;
    }

    struct epoll_event clientEvent;
    memset(&clientEvent, 0, sizeof(clientEvent));
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientSocket;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1) {
        logger.error("Failed to add client socket to epoll");
        ::close(clientSocket);
        return false;
    }

	std::string clientIP = getClientIP(clientAddr);
    User newUser(clientSocket, clientIP);
    addUser(newUser);
    return true;
}

bool Server::configurePoll(struct epoll_event &ev) {
    int serverFd = _serverSocket.getFd();
    if (serverFd <= 0) 
        throw ServerException("Invalid server socket file descriptor");

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1)
    {
        throw ServerException("Failed to add server socket to epoll");
    }
	return true;
}

void Server::handleClientData(int clientFd)
{
    char buf[512];
    std::memset(buf, 0, sizeof(buf));

    int recvBytes = recv(clientFd, buf, 511, 0);
    if (recvBytes <= 0)
    {
		close(clientFd);
		_users.erase(clientFd);
        return;
    }
    _users[clientFd].appendBuffer(std::string(buf));
    std::string &bufferRef = _users[clientFd].getBufferRef();
	std::string::size_type pos = bufferRef.find("\n");
	if (bufferRef.find("\r") != std::string::npos)
    	pos = bufferRef.find("\r\n");
    while (pos != std::string::npos)
    {
        std::string line = bufferRef.substr(0, pos);
        bufferRef.erase(0, pos + 2);
        std::ostringstream oss;
        oss << "Message from client " << clientFd << ": " << line;
        logger.info(oss.str());
        pos = bufferRef.find("\n");
		if (bufferRef.find("\r") != std::string::npos)
    		pos = bufferRef.find("\r\n");
        _commandHandler.processCommand(line, clientFd);
    }
}

void Server::addUser(User& user) {
    _users[user.getSocket()] = user;
}

std::map<int, User> Server::getUsers() {
    return _users;
}

Socket& Server::getSocket() {
    return _serverSocket;
}

int& Server::getEpollFd() {
    return _epollFd;
}

bool Server::getSignal() {
    return _isRunning;
}

void Server::setSignals() {
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
}

void Server::signalHandler(int signum)
{
	(void)signum;
	_isRunning = true;
}

std::string Server::getClientIP(sockaddr_in &clientAddr) const {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    return std::string(clientIP);
}

void Server::closeFds() {
    std::map<int, User>::iterator it = _users.begin();
    while (it != _users.end()){
        if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL) == -1) 
        {
            logger.error("Failed to remove client socket from epoll");
        }

        it->second.closeSocket();
        std::map<int, User>::iterator nextIt = it;
        ++nextIt;
        _users.erase(it);
        it = nextIt;
    }
}
