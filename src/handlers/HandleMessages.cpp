#include "../../include/handlers/HandleMessages.hpp"

void HandleMessages::handleMessage(int clientSocket) {
    char buf[4096];
    const std::string logFileName = "log.txt";

    // struct stat buffer;

    // if (stat(logFileName.c_str(), &buffer) != 0) {
    //     // Corrected: Using .c_str() to convert std::string to const char*
    //     std::ofstream createFile(logFileName.c_str()); // This works in C++98
    //     if (!createFile) {
    //         std::cerr << "Error creating the file " << logFileName << std::endl;
    //         return;
    //     }
    // }

    // // Corrected: Using .c_str() to convert std::string to const char*
    // std::ofstream logFile(logFileName.c_str(), std::ios::app); // This works in C++98
    // if (!logFile.is_open()) {
    //     std::cerr << "Failed to open log file." << std::endl;
    //     return;
    // }

    char buf[512];
    std::memset(buf, 0, sizeof(buf));

    int clientFd = _fds[index].fd;
    // int recvBytes = recv(clientFd, buf, 511, 0);
    // if (recvBytes <= 0)
    // {
    //     disconnectClient(index);
    //     return;
    // }
    _users[clientFd].appendBuffer(std::string(buf));
    std::string &bufferRef = _users[clientFd].getBufferRef();
    std::string::size_type pos = bufferRef.find("\r\n");
    while (pos != std::string::npos)
    {
        std::string line = bufferRef.substr(0, pos);
        bufferRef.erase(0, pos + 2);
        std::cout << "Client command: " << line << std::endl;
        _commandHandler.processCommand(line, clientFd);
        pos = bufferRef.find("\r\n");
    }
    // logFile.close();
}