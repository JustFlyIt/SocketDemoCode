/************************************************************************

 	File Name:   	ClientSocketExample.c

  	Description:	Socket file client - reads from a server socket
 					and writes data to a file

   Call:			ClientSocketExample 127.0.0.1 22222
   										host id   port number
*************************************************************************/
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE 600000
#define INPUT_FILE_BUF_SIZE 50

void error(const char *msg) {
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]) {
	int socketFileDescriptor, portNumber;
	ssize_t bytesRead; /* Number of bytes returned by read() and write() */
	struct sockaddr_in serverAddress;
	struct hostent *server;

	char userInputBuffer[INPUT_FILE_BUF_SIZE];

	// Check the number of arguments
	if (argc < 3) {
		fprintf(stderr, "Requires a host-name and port -  %s \n", argv[0]);
		exit(0);
	}

	// Get the port number
	portNumber = atoi(argv[2]);

	// Open the socket
	socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFileDescriptor < 0)
		error("ERROR opening socket");

	// Get the host name
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, host as specified by the command-line host-name doesn't exist.\n");
		exit(0);
	}

	// Zero out server address
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;

	bcopy((char *) server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);

	serverAddress.sin_port = htons(portNumber);

	if (connect(socketFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		error("Connection Error.");

	printf("Enter character to initiate file data transfer: ");
	bzero(userInputBuffer, INPUT_FILE_BUF_SIZE);    // Clear the input buffer
	fgets(userInputBuffer, INPUT_FILE_BUF_SIZE-1, stdin);   // Get user input

	// Send start message to server
	bytesRead = write(socketFileDescriptor, userInputBuffer, strlen(userInputBuffer));
	if (bytesRead < 0)
		error("ERROR writing to socket");

	/* Output file descriptor */
	int outputFileDescriptor = open("./OutputFile", O_WRONLY | O_CREAT, 0644);
	if (outputFileDescriptor == -1) {
		error("File open error");
		return 5;
	}

	// Setup for socket read of the file data from the server
	int loopCount = 1;
	long totalBytesRead = 0;
	long largestBytesRead = 0;
	long loopCountMaxBytesRead = 0;
	ssize_t bytesWritten; /* Number of bytes written by write() - file IO */
	char fileBuffer[BUF_SIZE];

	// Read the file data from the server
	while (loopCount) {
		bytesRead = read(socketFileDescriptor, fileBuffer, BUF_SIZE);
		if (bytesRead < 0) {
			error("ERROR reading from socket");
			break;
		}

		// Note largest number of bytes read at a time.
		if (largestBytesRead < bytesRead) {
			largestBytesRead = bytesRead;
			loopCountMaxBytesRead = loopCount;
		}

		// Update total number of file transfer bytes read from socket.
		totalBytesRead += bytesRead;

		printf("Loop Count: %d     Bytes Read: %ld    Total Bytes Read: %ld  \n", loopCount++, bytesRead, totalBytesRead);

		if (bytesRead == 0) {
			printf("\n\nFinished reading socket and writing to file.\n\n");
			printf("Largest number of bytes received from socket (read): %ld\n", largestBytesRead);
			printf("Occurred at loop count: %ld\n", loopCountMaxBytesRead);
			break;
		}

		bytesWritten = write(outputFileDescriptor, &fileBuffer, bytesRead);
		if (bytesRead != bytesWritten) {  	/* Write error */
			error("File write error");
			break;
		}
	}

	close(outputFileDescriptor);
	close(socketFileDescriptor);

	return 0;
}
