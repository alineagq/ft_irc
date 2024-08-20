// test_network.cpp
#include <gtest/gtest.h>
#include "../include/network/Socket.hpp"
#include <arpa/inet.h>
#include <unistd.h>

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

    sockaddr_in clientAddr;
    int clientSocket = serverSocket.accept(clientAddr);
    EXPECT_GT(clientSocket, 0);
}

TEST(SocketTest, CloseSocket) {
    Socket socket;
    ASSERT_TRUE(socket.create());
    socket.close();
    // No explicit assertion needed, just ensuring no exceptions are thrown
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}