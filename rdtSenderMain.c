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
	int portNum, proxyPortNum, sockFD;
	char *proxyHostName, *message;
	struct sockaddr_in proxAddress;
	struct sockaddr_in sendAddress;
	char inputMessage[256];
	bzero(inputMessage, 256); 
	int processedSeg = 0;
	

	int i = 0;
	
	if (argc != 4)
	{
		printf("Rerun rdtSender with the correct command line arguments\n");
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
	printf("message = %s\n", inputMessage);
	int messageLength = strlen(inputMessage);
	int numOfSegments = ceil((messageLength + 5)/10); //5 for the header
	printf("numofSegments = %d\n", numOfSegments);
	// Parse message into segments
/*	while (i < strlen(inputMessage) - 1)
	{	
		printf("in while\n");
		SegmentP thisSegment = createSegment(parseMessage(i, inputMessage));
		if (thisSegment == 0x00) break;
		printf("thisSegment->segMessage @ %d = %s\n", i, thisSegment->segMessage);
		free(thisSegment);
		sendMessage(sockFD, thisSegment, proxyHostName, proxyPortNum);
		i++;
	}*/

	while(i < (strlen(inputMessage) - 1))
	{
		SegmentP thisSegment = createSegment(parseMessage(i, inputMessage));
		//messageSent = parseMessage(i, inputMessage);
		//printf("segment Message = %s\n", thisSegment->segMessage);
		sendMessage(sockFD, thisSegment, proxyHostName, proxyPortNum);
		i++;
		free(thisSegment);
	}
	return 0;
}
