// test_network.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/network/TcpConnection.hpp"
#include "../include/network/Socket.hpp"
#include <arpa/inet.h>
#include <unistd.h>

// Mock class for Socket
class MockSocket {
public:
    MOCK_METHOD0(create, bool());
    MOCK_METHOD1(bind, bool(unsigned short port));
    MOCK_METHOD1(listen, bool(int backlog));
    MOCK_METHOD1(accept, int(sockaddr_in& clientAddr));
    MOCK_METHOD0(close, void());
};

// Mock class for TcpConnection
class MockTcpConnection {
public:
    MOCK_METHOD0(handleConnection, void());
    MOCK_METHOD0(handleClient, void());
};

// Test case for Socket class
TEST(SocketTest, CreateSocket) {
    MockSocket mockSocket;
    EXPECT_CALL(mockSocket, create())
        .Times(1)
        .WillOnce(testing::Return(true));

    EXPECT_TRUE(mockSocket.create());
}

TEST(SocketTest, BindSocket) {
    MockSocket mockSocket;
    EXPECT_CALL(mockSocket, bind(testing::_))
        .Times(1)
        .WillOnce(testing::Return(true));

    EXPECT_TRUE(mockSocket.bind(8080));
}

TEST(SocketTest, ListenSocket) {
    MockSocket mockSocket;
    EXPECT_CALL(mockSocket, listen(testing::_))
        .Times(1)
        .WillOnce(testing::Return(true));

    EXPECT_TRUE(mockSocket.listen(5));
}

TEST(SocketTest, AcceptConnection) {
    MockSocket mockSocket;
    sockaddr_in clientAddr;
    EXPECT_CALL(mockSocket, accept(testing::_))
        .Times(1)
        .WillOnce(testing::Return(1));

    EXPECT_EQ(mockSocket.accept(clientAddr), 1);
}

TEST(SocketTest, CloseSocket) {
    MockSocket mockSocket;
    EXPECT_CALL(mockSocket, close())
        .Times(1);

    mockSocket.close();
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

    TcpConnection connection(clientSocket, clientAddr);
    connection.handleClient();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}