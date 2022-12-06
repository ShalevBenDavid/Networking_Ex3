#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 86
#define IP_ADDRESS "127.0.0.1"
#define FILE_SIZE 1048576 // file size + 1 for the \0.
#define FILE_NAME "TextFile.txt"
int main() {
    //-------------------------------Read File-----------------------------

    FILE* file_pointer;
    file_pointer = fopen(FILE_NAME, "r");

    // Check if we were successful in opening file.
    if(file_pointer == NULL) {
        printf("Error in opening file : %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("File opened successfully!\n");
    }

    // Create array for holding the message.
    char message[FILE_SIZE] = {0};
    fread(message, sizeof(char), FILE_SIZE, file_pointer);

    // Closes the stream. All buffers are flushed.
    fclose(file_pointer);

    //-------------------------------Create Socket-----------------------------

    // Creates socket named "socketFD". FD for file descriptor.
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    // Check if we were successful in creating socket.
    if(socketFD == -1) {
        printf("Could not create socket : %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Socket created successfully!\n");
    }

    // Create sockaddr_in for IPv4 for holding ip address and port and clean it.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, '\0', sizeof(serverAddress));

    // Assign port and address to "serverAddress".
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT); // Short, network byte order.
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Convert address to binary.
    int rval = inet_pton(AF_INET, IP_ADDRESS, &serverAddress.sin_addr);
    if (rval <= 0)
    {
        printf("inet_pton() has failed!");
        return -1;
    }

    //Create connection with server.
    int connection = connect(socketFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    // Check if we were successful in connecting with server.
    if(connection == -1) {
        printf("Could not connect to server. connect() failed with error code: %d\n", errno);
        exit(EXIT_FAILURE); // Exit program and print EXIT_FAILURE (defined as 1 in stdlib.h).
    }
    else {
        printf("Connection with server established.\n");
    }
    // Close connection.
    close(socketFD);

    return 0;
}

