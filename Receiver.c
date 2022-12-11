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

#define PORT 8085
#define MAX_CONNECTIONS 300
#define FILE_SIZE 1048574
#define ID1 2781
#define ID2 8413

int recv_message(int);

double times[2][15] = {0.0}; // Global array for holding times. first row is for first half, second row is for second half.
int numOfTimes = 0; //  Global variable for number of times saved.

int main() {
    //----------------------------------Create TCP Connection---------------------------------
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
    while(true) {
        if(recv_message(clientSocket) == -1){
            printf("Exiting...\n");
            break;
        }
    }

    // print times
    for (int i = 0; i < numOfTimes; ++i) {
        printf("%d - First Half: %lf\tSecond Half: %lf\t\n", i+1, times[0][i], times[1][i]);
    }

    double sum1 = 0.0; // sum time for the first part
    double sum2 = 0.0; // sum time for the second part

    for (int i = 0; i < numOfTimes; ++i) {
        sum1 += times[0][i];
        sum2 += times[1][i];
    }

    double average1 = (double) sum1 / numOfTimes; // average time for the first part
    double average2 = (double) sum2 / numOfTimes; // average time for the second part

    printf("Average time for the first part: %lf\n", average1);
    printf("Average time for the first part: %lf\n", average2);
    printf("Average time for the file: %lf\n", (average1 + average2)/2.0);

    // Close socket.
    close(clientSocket);
}

// Method for receiving message from sender.
int recv_message(int clientSocket) {
    char buffer[FILE_SIZE + 1]; // Array for holding the message. His size is size + 1 for the \0.
    clock_t t; // Will help measure time.
    double time = 0.0; // Will hold time that elapsed in seconds.
    size_t receivedTotalBytes = 0;
    t = clock();  // Start time.

    // change CC
    char cc_reno[5] = {0};
    strcpy(cc_reno, "reno");
    if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, cc_reno, strlen(cc_reno)) != 0) {
        printf("error\n");
    }


    //----------------------------------Receive First Messages---------------------------------
    while (receivedTotalBytes != FILE_SIZE / 2) {
        bzero(buffer, FILE_SIZE + 1); // Clean buffer.
        ssize_t receivedBytes = recv(clientSocket, buffer, FILE_SIZE / 2 - receivedTotalBytes, 0);
        if (receivedBytes <= 0) break; // Break if we got an error (-1) or peer closed half side of the socket (0).
        receivedTotalBytes += receivedBytes; // Add the new received bytes to the total bytes received.
//        printf("%s", buffer);
        // Check if we need to exit.
        if(strcmp(buffer, "exit") == 0) return -1;
    }
    t = clock() - t;  // End time.
    time = ((double) t) / CLOCKS_PER_SEC; // Calculate the elapsed time in seconds.
    times[0][numOfTimes] = time;
    printf("\n----------------------------\nFinished receiving first half\n------------------------------\n");

    //----------------------------------Send authentication---------------------------------
    char xor[6] = {0};
    sprintf(xor, "%d", ID1 ^ ID2);
    send(clientSocket, xor, 5, 0);

    //----------------------------------Receive Second Messages---------------------------------
    char cc_cubic[6] = {0};
    strcpy(cc_cubic, "cubic");
    if (setsockopt(clientSocket, IPPROTO_TCP, TCP_CONGESTION, cc_cubic, strlen(cc_cubic)) != 0) {
        printf("error\n");
    }
    bzero(buffer, FILE_SIZE + 1);
    receivedTotalBytes = 0;
    t = clock();
    while (receivedTotalBytes != FILE_SIZE / 2) {
        bzero(buffer, FILE_SIZE+1);
        ssize_t receivedBytes = recv(clientSocket, buffer, FILE_SIZE / 2 - receivedTotalBytes, 0);
        if (receivedBytes <= 0) break; // break if we got an error (-1) or peer closed half side of the socket (0).
//        printf("%s", buffer);
        receivedTotalBytes += receivedBytes;
    }
    t = clock() - t;
    time = ((double) t) / CLOCKS_PER_SEC;
    times[1][numOfTimes] = time;
    printf("\n----------------------------\nFinished receiving second half\n------------------------------\n");
    numOfTimes++;
    return 0;
}