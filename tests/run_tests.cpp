// test_network.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/network/Socket.hpp"
#include "../include/network/TcpConnection.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sys/stat.h>

using ::testing::AtLeast;
using ::testing::Return;

class MockTcpConnection : public TcpConnection {
public:
    MockTcpConnection(int socket, const sockaddr_in& clientAddr)
        : TcpConnection(socket, clientAddr) {}

    MOCK_METHOD0(handleClient, void());
};

class TestTcpConnection : public TcpConnection {
public:
    TestTcpConnection(int socket, const sockaddr_in& clientAddr)
        : TcpConnection(socket, clientAddr) {}

    using TcpConnection::handleClient;
};

// Function to simulate a client connecting to the server
void simulateClientConnection(int port) {
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Attempt to connect to the server
    connect(clientSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    close(clientSock);
}

// Test case for Socket class
TEST(SocketTest, CreateSocket) {
    Socket socket;
    EXPECT_TRUE(socket.create());
}

TEST(SocketTest, BindSocket) {
    Socket socket;
    ASSERT_TRUE(socket.create());
    EXPECT_TRUE(socket.bind(8080));
}

TEST(SocketTest, ListenSocket) {
    Socket socket;
    ASSERT_TRUE(socket.create());
    ASSERT_TRUE(socket.bind(8080));
    EXPECT_TRUE(socket.listen(5));
}

TEST(SocketTest, AcceptConnection) {
    Socket serverSocket;
    ASSERT_TRUE(serverSocket.create());
    ASSERT_TRUE(serverSocket.bind(8080));
    ASSERT_TRUE(serverSocket.listen(5));

    // Start a thread to simulate a client connection
    std::thread clientThread(simulateClientConnection, 8080);

    sockaddr_in clientAddr;
    int clientSocket = serverSocket.accept(clientAddr);
    EXPECT_GT(clientSocket, 0);

    // Join the client thread to ensure it completes
    clientThread.join();
}

TEST(SocketTest, CloseSocket) {
    Socket socket;
    ASSERT_TRUE(socket.create());
    socket.close(); // No explicit assertion needed, just ensuring no exceptions are thrown
}

// Test cases for TcpConnection class
TEST(TcpConnectionTest, HandleConnectionCallsHandleClient) {
    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

    MockTcpConnection mockTcpConnection(1, clientAddr);
    EXPECT_CALL(mockTcpConnection, handleClient()).Times(1);

    mockTcpConnection.handleConnection();
}

TEST(TcpConnectionTest, HandleClientReceivesAndEchoesMessage) {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

    TestTcpConnection tcpConnection(sockets[0], clientAddr);

    std::thread clientThread([&sockets]() {
        const char* message = "Hello, Server!";
        send(sockets[1], message, strlen(message), 0);

        char buffer[4096];
        int bytesReceived = recv(sockets[1], buffer, sizeof(buffer) - 1, 0);
        buffer[bytesReceived] = '\0';

        EXPECT_STREQ(buffer, message);
        close(sockets[1]);
    });

    tcpConnection.handleClient();
    clientThread.join();
}

TEST(TcpConnectionTest, HandleClientCreatesAndWritesToLogFile) {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

    TestTcpConnection tcpConnection(sockets[0], clientAddr);

    std::thread clientThread([&sockets]() {
        const char* message = "Log this message!";
        send(sockets[1], message, strlen(message), 0);
        close(sockets[1]);
    });

    tcpConnection.handleClient();
    clientThread.join();

    std::ifstream logFile("log.txt");
    ASSERT_TRUE(logFile.is_open());

    std::string logContent((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    EXPECT_NE(logContent.find("Received: Log this message!"), std::string::npos);

    logFile.close();
    remove("log.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}