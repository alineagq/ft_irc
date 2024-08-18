#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <string>
#include <arpa/inet.h> // Add this line
#include <unistd.h>
#include <string.h>

int main(){

    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1){
        std::cerr << "Can't create a socket!" << std::endl;
        return -1;
    }

    // Bind the socket to an IP / port

    sockaddr_in hint;

    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){
        std::cerr << "Can't bind to IP/port" << std::endl;
        return -2;
    }

    // Mark the socket for listening in

    if (listen(listening, SOMAXCONN) == -1){
        std::cerr << "Can't listen!" << std::endl;
        return -3;
    }

    // Accept a call

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1){
        std::cerr << "Problem with client connecting!" << std::endl;
        return -4;
    }

    // Close the listening socket

    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result){
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    // While receiving display message, echo message

    char buf[4096];
    while (true){
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1){
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }

        if (bytesReceived == 0){
            std::cout << "The client disconnected" << std::endl;
            break;
        }

        // Display message
        std::cout << "Received: " << std::string(buf, 0, bytesReceived) << std::endl;

        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }

    // Close the socket

    close(clientSocket);
    return 0;
}