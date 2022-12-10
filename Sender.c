#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define PORT 8084
#define IP_ADDRESS "127.0.0.1"
#define FILE_SIZE 1048574
#define FILE_NAME "TextFile.txt"
#define ID1 2781
#define ID2 8413

// Method for sending message.
size_t send_message(char message[], int socketFD) {
    size_t totalLengthSent = 0;
    while (totalLengthSent < FILE_SIZE/2) {
        ssize_t bytes = send(socketFD, message + totalLengthSent, FILE_SIZE/2 - totalLengthSent, 0);
        totalLengthSent += bytes;
    }
    return 1;
}


int main() {
    //-------------------------------Read File-----------------------------

    FILE* file_pointer;
    file_pointer = fopen(FILE_NAME, "r");

    // Check if we were successful in opening file.
    if(file_pointer == NULL) {
        printf("Error in opening file! -> fopen() failed with error code: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("File opened successfully.\n");
    }

    // Create array for holding the message.
    char message[FILE_SIZE + 1] = {0}; // file size + 1 for the \0.
    fread(message, sizeof(char), FILE_SIZE, file_pointer);

    // Closes the stream. All buffers are flushed.
    fclose(file_pointer);

    //-------------------------------Create TCP Connection-----------------------------

    // Creates socket named "socketFD". FD for file descriptor.
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    // Check if we were successful in creating socket.
    if(socketFD == -1) {
        printf("Could not create socket! -> socket() failed with error code: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Socket created successfully.\n");
    }

    // Create sockaddr_in for IPv4 for holding ip address and port and clean it.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, '\0', sizeof(serverAddress));

    // Assign port and address to "serverAddress".
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT); // Short, network byte order.
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Convert address to binary.
    if (inet_pton(AF_INET, IP_ADDRESS, &serverAddress.sin_addr) <= 0)
    {
        printf("Failed to convert IPv4 address to binary! -> inet_pton() failed with error code: %d\n", errno);
        return -1;
    }

    //Create connection with server.
    int connection = connect(socketFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    // Check if we were successful in connecting with server.
    if(connection == -1) {
        printf("Could not connect to server! -> connect() failed with error code: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Connection with server established.\n");
    }

    //-------------------------------Send Message---------------------------------
    int ans;
    while (true) {
        printf("Do you want to send file? Enter Y for Yes or N for No.\n");
//        ans = (char) getchar();
        while((ans = getchar()) == '\n' || getchar() == EOF);
        if (ans == 'N') {break;}
        if (send_message(message, socketFD) == -1) {
            printf("Failed to send first half of the message!\n");
        }
        else {
            printf("Sent the first half of the message.\n");
        }

        if(send_message(message + FILE_SIZE / 2, socketFD) == -1) {
            printf("Failed to send second half of the message!\n");
        }
        else {
            printf("Sent the second half of the message.\n");
        }
    }
    printf("Exiting...\n");

    //-------------------------------Close connection-----------------------------
    close(socketFD);
    printf("Connection closed!\n");

    return 0;
}

