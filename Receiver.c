#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8084
#define MAX_CONNECTIONS 300
#define FILE_SIZE 1048574
#define ID1 2781
#define ID2 8413
#define SEG_SIZE 4096


void recv_message(int);
double averageTimes = 0.0; // Global variable for average times.
double times[15]; // Array for holding times.
int numOfTimes = 0;

int main() {
    // Creates endpoint for communication named "socketFD". FD for file descriptor.
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);

    // Check if we were successful in creating socketFD.
    if (SocketFD == -1) {
        printf("Could not create socket! -> socket() failed with error code: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Socket created successfully.\n");
    }

    // Check if address is already in use.
    int enableReuse = 1;
    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(enableReuse)) <  0) {
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
        printf("Failed to bind address && port to socket! -> bind() failed with error code: %d\n", errno);
        close(SocketFD); // close the socket.
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Binding was successful!\n");
    }

    // Make server start listening and waiting and check if listen() was successful.
    if (listen(SocketFD, MAX_CONNECTIONS) == -1) { // We allow no more than MAX_CONNECTIONS queue connections requests.
        printf("Failed to start listening! -> listen() failed with error code : %d\n", errno);
        close(SocketFD); // close the socket.
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    printf("Waiting for incoming TCP-connections...\n");

    // Create sockaddr_in for IPv4 for holding ip address and port of client and cleans it.
    struct sockaddr_in clientAddress;

    // Accept and establish new TCP connections.
   //while(true) {
        memset(&clientAddress, 0, sizeof(clientAddress));
        unsigned int clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(SocketFD, (struct sockaddr*)&clientAddress, &clientAddressLen); // Accept connection.
        if (clientSocket == -1) {
            printf("Failed to accept connection. -> accept() failed with error code: %d\n", errno);
            close(SocketFD);
            close(clientSocket);
            exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
        }
        else {
            printf("Connection established.\n");
        }

        //----------------------------------Receive Messages---------------------------------
        recv_message(clientSocket);
        close(clientSocket);
    }
//}

// Method for receiving message from sender.
void recv_message(int clientSocket) {
    char buffer[FILE_SIZE + 1]; // file size + 1 for the \0.
    clock_t t;
    double time = 0.0;
    size_t recvTotalLength = 0;
    t = clock();  // Start time.
    // receive first half
    while (recvTotalLength != FILE_SIZE / 2) {
        bzero(buffer, FILE_SIZE+1);
        ssize_t receivedBytes = recv(clientSocket, buffer, FILE_SIZE / 2 - recvTotalLength, 0);
        if (receivedBytes <= 0) break; // break if we got an error (-1) or peer closed half side of the socket (0).
        printf("%s", buffer);
        recvTotalLength += receivedBytes;
    }
    t = clock() - t;  // End time.
    time = ((double) t) / CLOCKS_PER_SEC; // calculate the elapsed time in seconds.
    averageTimes += time;
    numOfTimes++;
    printf("\n\n\n\n----------------------------\nfinished receiving first half\n------------------------------\n\n\n\n");
    bzero(buffer, FILE_SIZE+1);
    recvTotalLength = 0;
    // receive second half
    while (recvTotalLength != FILE_SIZE / 2) {
        bzero(buffer, FILE_SIZE+1);
        ssize_t receivedBytes = recv(clientSocket, buffer, FILE_SIZE / 2 - recvTotalLength, 0);
//        receivedBytes += recv(clientSocket, buffer, 2, 0);
        if (receivedBytes <= 0) break; // break if we got an error (-1) or peer closed half side of the socket (0).
        printf("%s", buffer);
        recvTotalLength += receivedBytes;
    }
    printf("\n----------------------------\nfinished receiving second half\n------------------------------\n");
}