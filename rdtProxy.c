/* * * * * * * */
/* rdtProxy.c */
/* * * * * * * */    

/* Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * Implementation of functions described by rdtProxy.h
 */

#include "rdtSender.h"
#include "rdtProxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

/*
 * Create and bind the socket
 */
int sockCreation(char *hostName, int port, struct sockaddr_in *dest)
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
 * Print the host information to the terminal
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
 * Display the port information to the screen
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
	struct sockaddr_in printSock;
    socklen_t addrLen = sizeof(struct sockaddr);
    getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
    fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}

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
	return sockfd;
}
int sentMessage(int sockFD, sentSegmentP *thisSegment, char * serverName, int serverPort)
{
	printf("Receiver->segMessage: %s\n", thisSegment->segMessage);
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
    {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    return errorCheck;
}

/*
 * 'Roll dice' to see if segment is lost, delayed or corrupt
 *
 * Paramters :
 * pLost    - User entered percentage lost
 * pDelayed - User entered perecentage delayed
 * pError   - User entered percentage error (corrupt packet)
 * 
 * Returns :
 * 0 - Neither lost, delayed or an error in packet transmission
 * 1 - Packet is lost
 * 2 - Packet is delayed
 * 3 - Error with packet (corrupt)
 */
int isLostDelayedCorrupt(double lost, double delayed, double error)
{
    int randNum, pLost, pDelayed, pError;
    randNum = rand() % 100;
    pLost = (int) lost;
    pDelayed = (int) delayed;
    pError = (int) error;
    printf("%d - ", randNum);

    if (randNum < pLost)
    {
        printf("Lose this packet\n");
        return 1;
    }
    if (randNum < pDelayed)
    {
        printf("Delay this packet\n");
        return 2;
    }
    if (randNum < pError)
    {
        printf("Corrupt this packet\n");
        return 3;
    }
    else
    {
        printf("Pass packet to Receiver as is\n");
        return 0;
    }
}
void *delayFunc()
{
    
    
}
/*
 * Appropriately handle the result of isLostDelayedCorrupt()
 */
void handleLDC(int LDC, sentSegmentP *thisSegment, int proxSockFD, char *rcvHostName, int rcvPort, struct sockaddr *senderAddress, socklen_t addr_size)
{
    struct sockaddr_in rcvAddress;
    socklen_t rcvaddr_size = sizeof(struct sockaddr);
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr)
    pthread_attr_setdetachedstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, delayFunc, NULL);
    // Delay packet (need to dispatch a thread)
    if (LDC == 2)
        ;
    // 'Corrupt' packet
    if (LDC == 3)
        thisSegment->isCorrupt = 1; 
     // Always send so long as packet isn't 'lost'
    if (LDC != 1)
     {
        sentMessage(proxSockFD, thisSegment, rcvHostName, rcvPort);
        sentSegmentP *rcvSegment = malloc(sizeof(sentSegmentP));
        printf("proxySeg->corrupt = %d\n", thisSegment->isCorrupt);
        recvfrom(proxSockFD, rcvSegment, sizeof(sentSegmentP), 0, (struct sockaddr *)&rcvAddress, &rcvaddr_size);
        printf("Passing ack from Receiver to Sender\n");
        //rcvSegment->isCorrupt = thisSegment->isCorrupt;
        printf("rcvSegment->corrupt = %d\n\n", rcvSegment->isCorrupt);
        int errorCheck = sendto(proxSockFD, thisSegment, sizeof(sentSegmentP), 0, senderAddress, addr_size);
        if (errorCheck < 0)
        {
            fprintf(stderr, "%s\n", strerror(errno));
        }
        free(rcvSegment);
    }
    // Request sender to resend 'lost' packet by forcing timeout
    else
    {
        printf("puttin her to sleep\n");
        sleep(7);   
    }

    free(thisSegment);
}

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 */
void checkArgCount(int argc)
{
    if (argc != 7)
    {
        printf("Run with <rdtProxy port> <Receiver Host Name> <rdtReciever PortNum> <Lost Percent> <Delayed Percent> <Error Percent>\n");
        exit(-1);
    }
}

/*
 * Make sure LDC does not exceed 99%
 */
void checkLDCRange(int lost, int delayed, int corrupt)
{
    if (lost + delayed + corrupt > 98)
    {
        printf("Every packet will be lost, delayed or corrupt, please rerun rdtProxy with values summing up to less than 100\n");
        exit(-1);
    }
}