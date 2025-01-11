
#include "../../include/handlers/HandleUser.hpp"

HandleUser::HandleUser() {}

HandleUser::~HandleUser() {}

bool HandleUser::configureClient(IServer& server, int clientSocket, int epollFd) {
    //configuring the client socket to be non-blocking and adding it to the epoll
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode for clientSocket" << std::endl;
        ::close(clientSocket);
        return false;
    }

    struct epoll_event clientEvent;
    memset(&clientEvent, 0, sizeof(clientEvent));
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientSocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1) {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        ::close(clientSocket);
        return false;
    }

    server.addUser(User(clientSocket));
    std::cout << "Client successfully added to epoll" << std::endl;
    return true;
}