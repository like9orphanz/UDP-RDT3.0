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
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
	struct sockaddr_in printSock;
	socklen_t addrLen = sizeof(struct sockaddr);
    getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
	fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}

/*
 * Handle incorrect or invalid formatting
 */
void unknownError(char *buffer, char *rcvString)
{
	bzero(rcvString, 256);
	strcpy(rcvString, "<error>unknown format</error>");
}

/*
 * Wrap the message in appropriate XML tags
 */
void changeEcho(char * buffer, char * rcvString)
{
	int i = 0, length = 0;
	char reply[256];
	char *tp;
	char store[256];
	bzero(rcvString, 256);
	bzero(store, 256);
	bzero(reply, 256);
	strcpy(rcvString, buffer);
	tp = strstr(rcvString, ">");
	tp = tp + sizeof(char);
	memmove(rcvString, tp, 256);
	length = strlen(rcvString);
	char temp = '\0';
	while(i < length)
	{
		temp = rcvString[i];
		if(temp == '<')
			i = length;
		else
			store[i] = temp;
		i++;
	}
	strcpy(rcvString, store);
	strcpy(reply, "<reply>");
	strcat(reply, rcvString);
	strcat(reply, "</reply>");
	bzero(rcvString, 256);
	strcpy(rcvString, reply);
}

/*
 * Wrap the load avg in appropriate XML tags
 */
void changeLoadAvg(char * buffer, char * rcvString)
{
	char store[256];
	bzero(store, 256);
	strcpy(rcvString, "<replyLoadAvg>");
	int errorCheck = getLoad(store);
	if(errorCheck == -1)
	{
		fprintf(stderr, "Failed getting load aver\n");
		exit(-1);
	}
	strcat(rcvString, store);
	strcat(rcvString, "</replyLoadAvg>");
}

/*
 * Reads client message and responds accordingly
 */
int processInfo(char *buffer, char *rcvString)
{
	int errorCheck = 0, response = 0;
	int length = strlen(buffer);
	if(length > 256)
		return response;
	bzero(rcvString, 256);
	if((errorCheck = pShutdown(buffer)) == 0)
		response = 1;
	else if((errorCheck = echo(buffer)) != 0)
		changeEcho(buffer, rcvString);
	else if((errorCheck = loadAvg(buffer)) != 0)
		changeLoadAvg(buffer, rcvString);
	else
		unknownError(buffer, rcvString);
	
	return response;
}

/*
 * Echo the message back to the client
 */
int echo(char * rcvString)
{
	int response = 0;
	char dest[256];
	bzero(dest, 256);
	//puts the first 6 chars in the dest array
	strxfrm(dest, rcvString, 6);
	char *end = strstr(rcvString, "</echo>");
        if(end != NULL)
	{
		if((strcmp(dest, "<echo>") == 0) && (strcmp(end, "</echo>") == 0))
			response = 1;
	}
	return response;
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
 * Calculate the average server load
 */
int loadAvg(char rcvString[256])
{
	if((strcmp(rcvString, "<loadavg/>")) == 0)
		return 1;
	else
		return 0;
}

/* 
 * Get the current load on the server
 */
int getLoad(char *store)
{
	double loadAvg[3];
	bzero(store, 256);
	int errorCheck = 0;
	errorCheck = getloadavg(loadAvg, 3);
	if(errorCheck != -1)
	{
		sprintf(store, "%f:%f:%f", loadAvg[0], loadAvg[1], loadAvg[2]);
	}
	return errorCheck;
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
 */
SegmentP *createSegment(int i, char *parsedChars, SegmentP *thisSegment)
{
	if (parsedChars == 0x00) return  0x00;


	thisSegment->ack = i%2;
	thisSegment->seqNum = i;
	thisSegment->messageSize = strlen(parsedChars);

	//thisSegment->segMessage = parsedChars;
	strcpy(thisSegment->segMessage, parsedChars);
	free(parsedChars);
	//printf("after free and before return = %s\n", thisSegment->segMessage);
	return thisSegment;
}

/*
 * Parse the message obtained from user
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
 */ 
int sendMessage(int sockFD, SegmentP *thisSegment, char * serverName, int serverPort)
{
	//printf("sendMessage function: %s\n", thisSegment->segMessage);
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
  */
void handleTimerResult(int sockFD, struct sockaddr_in proxAddress, SegmentP *rcvSegment, SegmentP *thisSegment, char * serverName, int serverPort, int selectVal)
{
	socklen_t addr_size = sizeof(proxAddress);
	if (selectVal == 1)
	{
		recvfrom(sockFD, rcvSegment, sizeof(SegmentP), 0, (struct sockaddr *)&proxAddress, &addr_size);
		printf("receivery sent back ack\n");
		printf("ack after i recvfrom = %d\n", rcvSegment->ack);	
	}
	else
		printf("Ack wait timed out, resending packet\n");
		//sendMessage(sockFD, thisSegment, serverName, serverPort);
}

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
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
 * Get the message to be sent to receiver from the user 
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
