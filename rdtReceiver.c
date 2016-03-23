/*
 *	File Name: TCPclient.c
 *	Author: Joshua Wright
 *	Project: 1
 *
 */


#include "rdtReceiver.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 *	This program is a TCPclient that communicates to TCPserver. This program uses TCPmain 
 *	to send a certain ammount of messages and tests to see if those messages
 *	fit the criteria that the server replies with
 */

/*
 * Creates a datagram socket, returns the socket file descriptor
 */
int createSocket()
{
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fprintf(stderr, "ERROR opening socket");

	struct sockaddr_in printSock;
	socklen_t addrSize = sizeof(struct sockaddr_in);
	getsockname(sockfd, (struct sockaddr *)&printSock, &addrSize);
	fprintf(stderr, "Sock Port: %d\n", ntohs(printSock.sin_port));
	return sockfd;
}

/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sockFD     - the socket identifier
 * request    - the request to be sent encoded as a string
 * serverName - the ip address or hostname of the server given as a string
 * serverPort - the port number of the server
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sockFD, char * request, char * serverName, int serverPort)
{
	printf("Sending request\n");
	printf("Request: %s\n", request);
	int errorCheck = 0;
	struct hostent * htptr;
	struct sockaddr_in dest;

	if((htptr = gethostbyname(serverName)) == NULL)
	{
		fprintf(stderr, "Invalid host name\n");
		return -1;
	}

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(serverPort);
	dest.sin_addr = *((struct in_addr *)htptr->h_addr);	

	errorCheck = sendto(sockFD, request, strlen(request), 0, (const struct sockaddr *)&dest, sizeof(dest));
	
	return errorCheck;
}

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sockfd    - the socket identifier
 * response  - the server's response as an XML formatted string to be filled in by this function into the specified string array
 * size      - the size of the buffer for storing a response from the server
 * 
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sockFD, recvSegmentP *thisSegment)
{
	int errorCheck = 0;
	errorCheck = recvfrom(sockFD, thisSegment, sizeof(recvSegmentP), 0, NULL, NULL);
	return errorCheck;	
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char * response)
{
	printf("Response is: %s\n", response);
}

/*
 * Closes the specified socket
 *
 * sockFD - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sockFD)
{
	int errorCheck = 0;
	errorCheck = close(sockFD);
	return errorCheck;
}

/*
 * Print host information to the terminal
 */ 
void printHostInfo()
{
 	char hostname[1024];
    	hostname[1023] = '\0';
	struct hostent * hostptr;
	gethostname(hostname, 1023);
	//find the ip address
	hostptr = gethostbyname(hostname);
	fprintf(stderr, "Host Name: %s\n", hostname);
	fprintf(stderr, "IP address: %s\n", inet_ntoa(*(struct in_addr*)hostptr->h_addr));
}

/*
 * Create and bind socket
 */
int sockCreation(int port, struct sockaddr_in *address)
{
	int sock_ls;
	
	memset(address, 0, sizeof(*address));
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons(port);

	//creates a socket
	if((sock_ls = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
                fprintf(stderr, "Error: listen sock failed!");
                exit(1);
        }

	//binds the socket
	if(bind(sock_ls, (struct sockaddr *)address, sizeof(*address)) < 0)
        {
            fprintf(stderr, "Error binding\n");
            close(sock_ls);
            exit(1);
        }

	return sock_ls;
}

/*
 * Display the port information
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
	struct sockaddr_in printSock;
    socklen_t addrLen = sizeof(struct sockaddr);
    getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
    fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}
