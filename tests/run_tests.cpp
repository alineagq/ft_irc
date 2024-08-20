// test_network.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/network/Socket.hpp"
#include "../include/network/TcpConnection.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>


class MockTcpConnection {
public:
    MOCK_METHOD0(handleConnection, void());
    MOCK_METHOD0(handleClient, void());
};

// Derived class to expose protected method for testing
class TestTcpConnection : public TcpConnection {
public:
    using TcpConnection::TcpConnection; // Inherit constructors
    using TcpConnection::handleClient;  // Expose protected method
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

// Test case for TcpConnection class
TEST(TcpConnectionTest, HandleConnection) {
    int clientSocket = 1;
    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

    TcpConnection connection(clientSocket, clientAddr);
    connection.handleConnection();
}

TEST(TcpConnectionTest, HandleClient) {
    int clientSocket = 1;
    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

    TestTcpConnection connection(clientSocket, clientAddr);
    connection.handleClient();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}