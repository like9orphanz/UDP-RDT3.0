#include "rdtSender.h"
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
#include <math.h>

int main(int argc, char *argv[])
{	
	int portNum, proxyPortNum, sockFD, i = 0;
	char *proxyHostName, *message;
	struct sockaddr_in proxAddress, sendAddress;
	socklen_t addr_size = sizeof(proxAddress);
	char inputMessage[256];
	bzero(inputMessage, 256); 
	
	if (argc != 4)
	{
		printf("Run with <rdtSender port> <proxyHostName> <rdtProxy port>\n");
		exit(-1);
	}
	
	proxyHostName = (char *) malloc (sizeof(char) * 1024);
	portNum = atoi(argv[1]);
	strcpy(proxyHostName, argv[2]);
	proxyPortNum = atoi(argv[3]);
	sockFD = createSocket();
	int sendSockFD = sockCreation(proxyHostName, portNum, &sendAddress);
        printHostInfo();
        portInfo(&sendAddress, sendSockFD);
	printf("Enter a Message: ");
	fgets(inputMessage, 256, stdin);
	inputMessage[strlen(inputMessage)-1] = '\0';

	while(i < (strlen(inputMessage) / 4))
	{
		SegmentP *thisSegment = malloc(sizeof(SegmentP));
		thisSegment = createSegment((parseMessage(i, inputMessage)), thisSegment);
		printf("segment Message = %s\n", thisSegment->segMessage);
		sendMessage(sockFD, thisSegment, proxyHostName, proxyPortNum);
		free(thisSegment);
		i++;
	}
	
	SegmentP *recvSegment = malloc(sizeof(SegmentP));
	printf("ack before i recvfrom = %d\n", recvSegment->ack);
	recvfrom(sockFD, recvSegment, sizeof(SegmentP), 0, (struct sockaddr *)&proxAddress, &addr_size);

	printf("proxy sent back ack\n");
	printf("ack after i recvfrom = %d\n", recvSegment->ack);
	return 0;
}
