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
#include <sys/times.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

pthread_mutex_t lock;

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

    //printf("destName = %s\n", serverName);
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
int isLostDelayedCorrupt(double lost, double delayed, double error, int duplicate)
{
    int randNum, pLost, pDelayed, pError;
    randNum = rand() % 100;
    pLost = (int) lost;
    pDelayed = (int) delayed;
    pError = (int) error;
    
    if (duplicate == 1) 
    {
        printf("Pass packet to Receiver as is\n");
        return 0;
    }

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

/*
 * Delay the packet
 */
void *delayFunc(void *data)
{
    //fd_set set;
    //struct timeval timeout;
   // timeout.tv_sec = 2;
    //timeout.tv_usec = 0; 
    struct sockaddr_in rcvAddress;
    //struct sockaddr_in proxAddress;
    socklen_t rcvaddr_size = sizeof(struct sockaddr);
	threadP *threadStuff = (threadP *)data;
    threadStuff->isDone = 0;
    sentSegmentP *thisSegment = malloc(sizeof(sentSegmentP));
    sentSegmentP *rcvSegment = malloc(sizeof(sentSegmentP));  
    thisSegment->ack = threadStuff->ack;
    thisSegment->seqNum = threadStuff->seqNum;
    thisSegment->isCorrupt = threadStuff->isCorrupt;
    strcpy(thisSegment->segMessage, threadStuff->segMessage);
    	printf("above sleeping thread\n");
	sleep(6);
    printf("did i make it here in the thread\n");
	printf("ack in threadFunc = %d\n", thisSegment->ack);
    pthread_mutex_lock(&lock);
    sentMessage(threadStuff->sockFD, thisSegment, threadStuff->destName, threadStuff->destPort);
    
    //FD_ZERO(&set);
    //FD_SET(proxSockFD, &set);
    //int selectVal = select(proxSockFD + 1, &set, NULL, NULL, &timeout);
    //if (selectVal == -1)
    //{
        //perror("select failed\n");
        //exit(-1);
    //}
    //if (selectVal == 1)
    //{    
        //recvfrom(threadStuff->sockFD, rcvSegment, sizeof(sentSegmentP), 0, (struct sockaddr *)&rcvAddress, &rcvaddr_size);
        //printf("Passing ack = %d from Receiver to Sender in thread\n\n", rcvSegment->ack);
        //rcvSegment->isCorrupt = thisSegment->isCorrupt;
        ///int errorCheck = sendto(threadStuff->sockFD, rcvSegment, sizeof(sentSegmentP), 0, threadStuff->sender, threadStuff->sender_size);
        //if (errorCheck < 0)
            //fprintf(stderr, "%s\n", strerror(errno));
    //}
	free(thisSegment);
    free(rcvSegment);
    threadStuff->isDone = 1;
    pthread_mutex_unlock(&lock);
	return NULL;
}

/*
 * Appropriately handle the result of isLostDelayedCorrupt()
 */
void handleLDC(int LDC, sentSegmentP *thisSegment, int proxSockFD, char *rcvHostName, int rcvPort, struct sockaddr *senderAddress, socklen_t addr_size, int duplicate, int portNum)
{
    struct sockaddr_in rcvAddress;
    socklen_t rcvaddr_size = sizeof(struct sockaddr);
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    threadP *threadStuff;

    // Delay packet (need to dispatch a thread)
    if (LDC == 2)
    {
        threadStuff = malloc(sizeof(threadP));

        threadStuff->destName = rcvHostName;
        threadStuff->destPort = rcvPort;
	    threadStuff->sockFD = proxSockFD;
	    threadStuff->ack = thisSegment->ack;
	    threadStuff->isCorrupt = thisSegment->isCorrupt;
	    threadStuff->seqNum = thisSegment->seqNum;
        threadStuff->sender = senderAddress;
        threadStuff->sender_size = addr_size;
        threadStuff->isDone = 0;
        threadStuff->portNumber = portNum;

        strcpy(threadStuff->segMessage, thisSegment->segMessage);
	    
        pthread_create(&tid, &attr, &delayFunc, (void *)threadStuff);

    }  	 
    // 'Corrupt' packet
    if (LDC == 3)
        thisSegment->isCorrupt = 1;
     // If the packet isn't lost, always receive info from the receiver.
     // If it's delayed it will be sent in that functin
    if (LDC != 1 && LDC != 2)
     {  
        fd_set set;
        struct timeval timeout;

        timeout.tv_sec = 2;
        timeout.tv_usec = 0; 

        pthread_mutex_lock(&lock);
        sentSegmentP *rcvSegment = malloc(sizeof(sentSegmentP));
        printf("ack =: %d\n", thisSegment->ack);
        sentMessage(proxSockFD, thisSegment, rcvHostName, rcvPort);

        FD_ZERO(&set);
        FD_SET(proxSockFD, &set);
        int selectVal = select(proxSockFD + 1, &set, NULL, NULL, &timeout);

        if (selectVal == -1)
        {
            perror("select failed\n");
            exit(-1);
        }

        if (selectVal == 1) 
        {
            recvfrom(proxSockFD, rcvSegment, sizeof(sentSegmentP), 0, (struct sockaddr *)&rcvAddress, &rcvaddr_size);
            printf("Passing ack = %d from Receiver to Sender\n\n", rcvSegment->ack);
            rcvSegment->isCorrupt = thisSegment->isCorrupt;
            int errorCheck = sendto(proxSockFD, rcvSegment, sizeof(sentSegmentP), 0, senderAddress, addr_size);
            if (errorCheck < 0)
                fprintf(stderr, "%s\n", strerror(errno));
        }

        pthread_mutex_unlock(&lock);
        
        free(rcvSegment);
        if (duplicate == 1)
        {
           	printf("made it here\n");
		 pthread_join(tid, NULL);
        }
    }
    // Request sender to resend 'lost' packet by forcing timeout
    if (LDC == 1 && LDC !=2)
    {
        printf("puttin her to sleep\n\n");
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
