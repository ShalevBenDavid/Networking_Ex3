#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>

#define SERVER_PORT 80
#define SERVER_IP_ADDRESS "172.0.0.1"
int main() {
    // Create listening socket.
    int listener = socket(AF_INET, SOCK_STREAM, 0);

    // Check if we were successful in creating listening socket.
    if (listener == -1) {
        printf("Could not create listening socket: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Listening socket created!\n");
    }

    // Check if address is already in use.
    int enableReuse = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof enableReuse) ==  -1) {
        printf("setsockopt() failed with error code: %d\n" , errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }

    // Create sockaddr_in for IPv4 for holding ip address and port and clean it.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    // Assign port and address to "serverAddress".
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT); // Short, network byte order.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Binding port and address to socket and check if binding was successful.
    if (bind(listener, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        printf("bind() failed with error code: %d\n", errno);
        close(listener); // close the socket.
        exit(EXIT_FAILURE);
    }
    else {
        printf("binding was successful!\n");
    }

    // Make server start listening and waiting and check if listen() was successful.
    if (listen(listener, 300) == -1) { // We allow no more than 300 queue connections requests.
        printf("listen() failed with error code : %d\n", errno);
        close(listener); // close the socket.
        exit(EXIT_FAILURE);
    }
    printf("Waiting for incoming TCP-connections...\n");

    // Create sockaddr_in for IPv4 for holding ip address and port of client and cleans it.
    struct sockaddr_in clientAddress;

    while(true) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        unsigned int clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listener, (struct sockaddr*)&clientAddress, &clientAddressLen);
        if (clientSocket == -1) {
            printf("listen failed with error code: %d\n", errno);
            close(listener);
            close(clientSocket);
            exit(EXIT_FAILURE);
        }
        else {
            printf("A new client connection accepted\n");
        }
    }

}