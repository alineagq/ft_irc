#include "../infrastructure/configuration/Logger.hpp"
#include "../domain/entities/Server.hpp"
#include "../entrypoint/utils/utils.hpp"

Logger logger("server.log", "server_error.log");

int main(int argc, char* argv[]) {

    validateArgs(argc, argv);
    int port = validatePort(atoi(argv[1]));
    try {
        Server server(logger, port, argv[2]);
        server.run();
    }
    catch (const std::exception& e){
        logger.error(e.what());
        return 1;
    }
    return 0;
}