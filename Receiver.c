#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdbool.h>

#define PORT 8888
#define MAX_SIZE 300
#define FILE_SIZE 1048575 // file size + 1 for the \0.

void recv_file(int);

int main() {
    // Creates socket named "socketFD". FD for file descriptor.
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);

    // Check if we were successful in creating socket.
    if (SocketFD == -1) {
        printf("Could not create socket: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Socket created successfully!\n");
    }

    // Check if address is already in use.
    int enableReuse = 1;
    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof enableReuse) ==  -1) {
        printf("setsockopt() failed with error code: %d\n" , errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }

    // Create sockaddr_in for IPv4 for holding ip address and port and clean it.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, '\0', sizeof(serverAddress));

    // Assign port and address to "serverAddress".
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT); // Short, network byte order.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Binding port and address to socket and check if binding was successful.
    if (bind(SocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        printf("bind() failed with error code: %d\n", errno);
        close(SocketFD); // close the socket.
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("binding was successful!\n");
    }

    // Make server start listening and waiting and check if listen() was successful.
    if (listen(SocketFD, MAX_SIZE) == -1) { // We allow no more than MAX_SIZE queue connections requests.
        printf("listen() failed with error code : %d\n", errno);
        close(SocketFD); // close the socket.
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    printf("Waiting for incoming TCP-connections...\n");

    // Create sockaddr_in for IPv4 for holding ip address and port of client and cleans it.
    struct sockaddr_in clientAddress;

    // Accept and establish new TCP connections.
    while(true) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        unsigned int clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(SocketFD, (struct sockaddr*)&clientAddress, &clientAddressLen); // Accept connection.
        if (clientSocket == -1) {
            printf("listen failed with error code: %d\n", errno);
            close(SocketFD);
            close(clientSocket);
            exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
        }
        else {
            printf("Connection established.\n");
        }
        write_file(clientSocket);
        close(clientSocket);
    }
}

// Method for receiving file from sender.
void recv_file(int clientSocket) {
    char buffer[FILE_SIZE];
    while (true) {
        int n = recv(clientSocket, buffer, FILE_SIZE/2, 0);
        if (n <= 0) { // If there are no messages or an error occurred, we can stop and return.
            return;
        }
        else {
            printf("%s", n, buffer);
        }
        bzero(buffer,
    }
}