#include <iostream>
#include <cstdlib>
#include <sstream>  // Include for std::ostringstream
#include <vector>
#include <sys/epoll.h>
#include <cerrno>
#include "../include/logger/Logger.hpp"
#include "../include/core/Server.hpp"
#include "../include/handlers/HandleUser.hpp"
#include "../include/handlers/HandleMessages.hpp"

Logger logger("server.log", "server_error.log");
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
    server.run();
    
    return 0;
}

