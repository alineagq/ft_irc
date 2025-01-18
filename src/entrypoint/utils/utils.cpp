#include "utils.hpp"
#include "../../infrastructure/configuration/Logger.hpp"

extern Logger logger;

void validateArgs(int argc, char* argv[]) {
    if (argc != 3) {
        std::ostringstream oss;
        oss << "Usage: " << argv[0] << " <port>";
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }
}

int validatePort(int port) {
    if (port <= 0 || port > 65535) {
        std::ostringstream oss;
        oss << "Invalid port number. Port must be between 1 and 65535.";
        logger.error(oss.str());
        exit(EXIT_FAILURE);
    }
    return port;
}