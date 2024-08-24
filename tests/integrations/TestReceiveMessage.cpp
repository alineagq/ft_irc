#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

void runServer(int port) {
    std::string serverCommand = "../ircserver " + std::to_string(port) + " > /dev/null 2>&1";
    int result = system(serverCommand.c_str());
    if (result != 0) {
        std::cerr << "Server command failed with exit code " << result << std::endl;
        std::cerr << "Check if the server executable exists and is executable." << std::endl;
    }
}

void runClient(int port) {
    std::string clientCommand = "echo 'Hello, Server!' | nc 127.0.0.1 " + std::to_string(port) + " > /dev/null 2>&1";
    int result = system(clientCommand.c_str());
    if (result != 0) {
        std::cerr << "Client command failed with exit code " << result << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string killCommand = "lsof -ti:" + std::to_string(port) + " | xargs kill -9 > /dev/null 2>&1";
    int killResult = system(killCommand.c_str());
    if (killResult != 0) {
        std::cerr << "Kill command failed with exit code " << killResult << std::endl;
    }
}

TEST(IntegrationTest, serverClientCommunication) {
    int port = 1759;
    std::string logFilePath = "log.txt";

    // Remove the log file if it exists
    std::remove(logFilePath.c_str());

    std::thread serverThread(runServer, port);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    serverThread.detach();

    std::thread clientThread(runClient, port);
    clientThread.detach();

    std::this_thread::sleep_for(std::chrono::seconds(4));

    std::ifstream logFile(logFilePath);
    if (!logFile) {
        std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        FAIL() << "Log file not found.";
    }
    std::string logContent((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    logFile.close();

    EXPECT_NE(logContent.find("Received: Hello, Server!"), std::string::npos) << "Expected message not found in server log";
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}