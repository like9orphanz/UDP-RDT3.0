/* * * * * * * */
/* rdtSender.c */
/* * * * * * * */    

/* Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * Implementation of functions described by rdtSender.h
 */

#include "rdtSender.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

/*
 * Handle the shut down signal
 */
void handler(int param)
{
	fprintf(stderr, "\nCTRL-C has been entered. The server is shut down.\n");
	exit(EXIT_SUCCESS);
}

/*
 * Prints the host information to the terminal
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
 * Displays port info with the getsockname() funciton
 *
 * serverAddress  - the struct containing the address info
 * sockfd         - file descriptor
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
	struct sockaddr_in printSock;
	socklen_t addrLen = sizeof(struct sockaddr);
    	getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
	fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}

/*
 * Gracefully shutdown the server
 */
int pShutdown(char * rcvString)
{
	int response = 1;
	if((strcmp(rcvString, "<shutdown/>")) == 0)
		response = 0;
	return response;
}

/*
 * Creates the listening socket
 */
int createSocket()
{	int sockfd;
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            fprintf(stderr, "ERROR opening socket");

        struct sockaddr_in printSock;
        socklen_t addrSize = sizeof(struct sockaddr_in);
        getsockname(sockfd, (struct sockaddr *)&printSock, &addrSize);
        return sockfd;
}

/*
 * Create the socket
 *
 * hostName - the name of the host
 * port     - the port of the host
 * dest     - struct containing the address info    
 */
int sockCreation(char * hostName, int port, struct sockaddr_in *dest)
{
	int sock_ls;

        memset(dest, 0, sizeof(*dest));
        dest->sin_family = AF_INET;
        dest->sin_addr.s_addr = htonl(INADDR_ANY);
        dest->sin_port = htons(port);

	if((sock_ls = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
            fprintf(stderr, "Error: listen sock failed!");
            exit(1);
        }

	if(bind(sock_ls, (struct sockaddr *)dest, sizeof(*dest)) < 0)
        {
            fprintf(stderr, "Error binding\n");
            close(sock_ls);
            exit(1);
        }
	return sock_ls;
}					

/*
 * Create a 'segment' structure, assign the pased string to segMessage
 *
 * i            - the count
 * parsedChars  - pointer to the segment
 */
SegmentP *createSegment(int i, char *parsedChars)
{	
	if (parsedChars == 0x00) return  0x00;

	SegmentP *thisSegment = (SegmentP *) malloc(sizeof(SegmentP));
	thisSegment->ack = i%2;
	
	if (thisSegment->ack == 0)
		thisSegment->seqNum = 1;
	else
		thisSegment->seqNum = 0;
	
	thisSegment->isCorrupt = 0;
	strcpy(thisSegment->segMessage, parsedChars);
	free(parsedChars);
	return thisSegment;
}

/*
 * Parse the message obtained from user
 *
 * count   - counter
 * message - containing the message input by the user
 */
char *parseMessage(int count, char *message)
{
	int i;
	int a = 0;
	char *parsedChars = malloc(256);

	for ((i = count * 4) ; (i < (count * 4) + 4) ; i++) 
	{	
		if (i != 0)
		{
			parsedChars[a] = message[i];
			a++;
		}
		else
		{
			parsedChars[a] = message[i];
			if (message[i + 1] == '\0') return 0x00;

			a++;
		}
	}
	return parsedChars;
}

/*
 * Send message to the proxy
 *
 * sockFD      - the file descriptor
 * thisSegment - the struct containing the header and payload
 * serverName  - the name of the proxy
 * serverPort  - the port for the proxy 
 */ 
int sendMessage(int sockFD, SegmentP *thisSegment, char * serverName, int serverPort)
{
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

    	errorCheck = sendto(sockFD, thisSegment, sizeof(SegmentP), 0, (const struct sockaddr *)&dest, sizeof(dest));

	if(errorCheck == -1)
		fprintf(stderr, "%s\n", strerror(errno));

    return errorCheck;
}

/*
 * Detects a timeout in receiving an ack from the receiver
 */
int runTimer(int sockFD)
{
	int selectVal;
 	fd_set set;
	struct timeval timeout;

	FD_ZERO(&set);
	FD_SET(sockFD, &set);
	timeout.tv_sec = 5;
	timeout.tv_usec = 0; 
	
	selectVal = select(sockFD + 1, &set, NULL, NULL, &timeout);

	if (selectVal == -1)
	{
		perror("select failed\n");
		exit(-1);
	}

	return selectVal;
}

/*
 * Appropriatly handles any valid output from runTimer()
 *
 * sockFD      - the file descriptor
 * proxAddress - struct containing the proxy info
 * rcvSegment  - the struct containing the recv payload and header
 * thisSegment - the struct containing the sent payload and header
 * serverName  - the name of the host
 * serverPort  - the port Number of the host
 * selectVal   - the value of the select function from the sender 
 */
int handleTimerResult(int sockFD, struct sockaddr_in *proxAddress, SegmentP *rcvSegment, SegmentP *thisSegment, char * serverName, int serverPort, int selectVal)
{
	socklen_t addr_size = sizeof(proxAddress);
	if (selectVal == 1)
	{
		recvfrom(sockFD, rcvSegment, sizeof(SegmentP), 0, (struct sockaddr *)&proxAddress, &addr_size);
		if (rcvSegment->isCorrupt)
		{
			printf("Packet was corrupted, resending\n");
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		printf("Ack wait timed out, resending packet\n");
		return 1;
	}
}

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 *
 * argc - the count of the amount of integers
 */
void checkArgCount(int argc)
{
	if (argc != 4)
	{
		printf("Run with <rdtSender port> <proxyHostName> <rdtProxy port>\n");
		exit(-1);
	}
}

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 */
char *getMessage()
{
	char *inputMessage = (char *) malloc(sizeof(char) * 256);
	bzero(inputMessage, 256);

	printf("Enter a Message: ");
	fgets(inputMessage, 256, stdin);
	inputMessage[strlen(inputMessage)-1] = '\0';
	return inputMessage;
}
