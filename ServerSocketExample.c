/************************************************************************

 	File Name:   	ServerSocketExample.c

  	Description:	Socket file Server - Sends data from a file to a client
					via a socket

   Call:			ServerSocketExample 2222    "some file name"
   										port	file name
										number
*************************************************************************/
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DATA_FILE_BUF_SIZE 600000
#define INPUT_BUFFER_SIZE 256

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {
	printf("\n\nPlatform Two (server) Process Started...\n\n\n");

	int listenSocketFileDescriptor, functionalSocketFileDescriptor, portNumber;
	socklen_t addrStructSize;
	char buffer[256];
	char inputFileName[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 3) {
		fprintf(stderr, "ERROR, missing port and or input file name\n");
		exit(1);
	}

	// create a socket
	listenSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocketFileDescriptor < 0)
		error("ERROR opening socket");

	// clear address structure
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portNumber = atoi(argv[1]);
	strcpy(inputFileName, argv[2]);

	printf("InputFileName: %s\n\n", inputFileName);
	int inputFileDescriptor; /* Input and output file descriptors */
	ssize_t numberOfBytesRead; /* Number of bytes returned by read() and write() */

	/* Create input file descriptor */
	inputFileDescriptor = open(inputFileName, O_RDONLY);
	if (inputFileDescriptor == -1) {
		perror("open");
		return 2;
	}

	// setup the host_addr structure for use in bind call
	// server byte order
	serv_addr.sin_family = AF_INET;

	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	// Convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(portNumber);

	// bind() call will bind  the socket to the current IP address on port, portNumber
	if (bind(listenSocketFileDescriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	// listen() call tells the socket to listen to the incoming connections.
	listen(listenSocketFileDescriptor, 5);

	// The accept() call actually accepts an incoming connection
	addrStructSize = sizeof(cli_addr);

	printf("Waiting for Platform One (client) to initiate transfer...\n\n");

	// accept() function writes connecting client's address info
	// into the address structure.  Size of address structure is addrStructSize.
	functionalSocketFileDescriptor = accept(listenSocketFileDescriptor, (struct sockaddr *) &cli_addr, &addrStructSize);
	if (functionalSocketFileDescriptor < 0) {
		printf("Error\n\n");
		error("ERROR on accept");
	}

	bzero(buffer, INPUT_BUFFER_SIZE);
	n = read(functionalSocketFileDescriptor, buffer, INPUT_BUFFER_SIZE-1);
	if (n < 0)
		error("Socket Read Error");

	printf("Client initiates data transfer start....: %s\n", buffer);

	printf("Starting file data transfer....\n\n");

	// Setup for socket read of the file data from the server
	int loopCount = 1;
	long totalBytesWritten = 0;
	long largestBytesWritten = 0;
	long maxLoopCountBytesWrite = 0;
	ssize_t numberOfBytesWritten; /* Number of bytes written by write() - socket IO */
	char fileBuffer[DATA_FILE_BUF_SIZE];

	// File transmit process
	while ((numberOfBytesRead = read(inputFileDescriptor, &fileBuffer, DATA_FILE_BUF_SIZE)) > 0) {
		numberOfBytesWritten = write(functionalSocketFileDescriptor, &fileBuffer, (ssize_t) numberOfBytesRead);
		if (numberOfBytesWritten < 0) {
			error("ERROR writing to socket");
			break;
		}

		// Note if largest number of bytes read at a time.
		if (largestBytesWritten < numberOfBytesRead) {
			largestBytesWritten = numberOfBytesRead;
			maxLoopCountBytesWrite = loopCount;
		}

		totalBytesWritten += numberOfBytesWritten;

		printf("Loop Count: %d     Bytes Written: %ld    Total Bytes Written: %ld  \n", loopCount++, numberOfBytesWritten, totalBytesWritten);
	}

	printf("\n\nFinished reading file and writing to socket.\n\n");
	printf("Largest number of bytes written to socket (write): %ld\n", largestBytesWritten);
	printf("Occurred at loop count: %ld\n\n", maxLoopCountBytesWrite);

	/* Close file descriptors */
	close(inputFileDescriptor);

	close(functionalSocketFileDescriptor);
	close(listenSocketFileDescriptor);

	return 0;
}
