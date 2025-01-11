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

    memset(buf, 0, sizeof(buf));

    int bytesReceived = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (bytesReceived == -1) {
        std::cerr << "There was a connection issue" << std::endl;
    }

    if (bytesReceived == 0) {
        std::cout << "The client disconnected" << std::endl;
        close(clientSocket);
    }

    std::string receivedMessage(buf, 0, bytesReceived);
    std::cout << "Received: " << receivedMessage << std::endl;
    // logFile << "Received: " << receivedMessage << std::endl;

    int bytesSent = send(clientSocket, buf, bytesReceived, 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending message back to client" << std::endl;
    }

    std::cout << "Echoed message back to client" << std::endl;

    // logFile.close();
}