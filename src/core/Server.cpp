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
    // Convert the success message to string using ostringstream
     if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for clientSocket" << std::endl;
        ::close(serverSocketFd);
        exit(EXIT_FAILURE);
    }
    std::ostringstream oss;
    oss << "Server listening on port " << port << std::endl;
    logger.info(oss.str());

    _commandHandler.init(&_users, &_channels, "password");
	_isRunning = false;
}

bool Server::_isRunning;

Server::~Server() {
	closeFds();
    close(_epollFd);
	std::cout << "epollFd: " << _epollFd << std::endl;
	std::cout << "SSocket: " << _serverSocket.getFd() << std::endl;
    _serverSocket.close();
}

bool Server::run() {
	
	setSignals();
	 // try {
    //     setSignals();
    // } catch (std::exception& e) {
    //     // logger.error("Failed to set signals: " + std::string(e.what()));
    //     exit(EXIT_FAILURE);
    // }
	struct epoll_event ev, events[512];
	if (!configurePoll(ev)) {
		// logger.error("Failed to configure poll");
		return false;
	}

	int serverFd = _serverSocket.getFd();
	std::cout << "running: " << _isRunning << std::endl;
	while (_isRunning == false) {
            sockaddr_in clientAddr;
            // wait for events on the epoll
            int numEvents = epoll_wait(_epollFd, events, 512, -1);
            if (numEvents == -1) {
                // logger.error("Failed to wait for events");
                return false;
            }
            //handle the events
            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == serverFd)
                {
                    int clientSocket = _serverSocket.accept(clientAddr);
                    if (clientSocket == -1) {
                        // logger.error("Failed to accept client connection: " + std::string(strerror(errno)));
                        continue;
                    }
                    if (!configureClient(clientSocket)) {
                        // logger.error("Failed to configure client");
                        continue;
                    }
                }
                else
                {
                    handleClientData(events[i].data.fd);
                }
            }
    };
	std::cout << "TESTE" << std::endl;
	return true;
}

bool Server::configureClient(int clientSocket) {
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for clientSocket" << std::endl;
        ::close(clientSocket);
        return false;
    }

    struct epoll_event clientEvent;
    memset(&clientEvent, 0, sizeof(clientEvent));
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientSocket;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1) {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        ::close(clientSocket);
        return false;
    }

    User newUser(clientSocket);
    addUser(newUser);
    std::cout << "Client successfully added to epoll" << std::endl;
    std::cout << "Received Client socket: " << clientSocket << std::endl;
    std::cout << "Server Client socket: " << newUser.getSocket() << std::endl;

    return true;
}

bool Server::configurePoll(struct epoll_event &ev) {
    int serverFd = _serverSocket.getFd();
    if (serverFd <= 0) {
        // logger.error("Invalid server socket file descriptor");
        exit(EXIT_FAILURE);
    }
    
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    // Add the server socket to the epoll
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1) {
        // logger.error("Failed to add server socket to epoll: " + std::string(strerror(errno)) + "\n");
        exit(EXIT_FAILURE);
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
        _users.erase(clientFd);
        close(clientFd);
        std::cout << "Client disconnected" << std::endl;
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
	std::cout << std::endl << "Signal Received!" << std::endl;
	_isRunning = true;
}

void Server::closeFds() {
    std::map<int, User>::iterator it = _users.begin();
    while (it != _users.end()) {
        if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL) == -1) {
            std::cerr << "Failed to remove client socket from epoll: "
                      << strerror(errno) << std::endl;
        }
        std::cout << "Closing socket: " << it->second.getSocket() << std::endl;
        it->second.closeSocket();

        // Pegamos o próximo iterador aqui
        std::map<int, User>::iterator nextIt = it;
        ++nextIt;

        // Agora podemos apagar usando 'it'
        _users.erase(it);

        // Continuamos a iteração com nextIt
        it = nextIt;
    }
}
