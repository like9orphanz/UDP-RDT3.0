/*
 * RDT-UDP3.0
 * rdtProxyMain.c
 *
 * Sam Stein
 * Joshua Wright
 *
 * Uses rdtProxy.c to pass messages from server to client 
 */
#include "rdtProxy.h" 
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

 int main(int argc, char *argv[])
{
	int portNum, rcvPort, sockFD;
	double lostPercent, delayedPercent, errorPercent;
	char *rcvHostName;
	struct sockaddr_in proxAddress; //local address
	struct sockaddr_in rcvAddress; // sender's address
	struct sockaddr_in senderAddress;
	socklen_t addr_size = sizeof(senderAddress);
/*
 	if (argc != 7)
	{
		printf("Rerun rdtProxy with correct command line arguments\n");
		exit (-1);
	}
*/
	rcvHostName = (char *) malloc(sizeof(char) * 1024);
	portNum = atoi(argv[1]); //local port
	strcpy(rcvHostName, argv[2]);
	rcvPort = atoi(argv[3]); 
	//lostPercent = atoi(argv[4]);
	//delayedPercent = atoi(argv[5]);
	//errorPercent = atoi(argv[6]);
	sockFD = createSocket();
	int proxSockFD = sockCreation(rcvHostName, portNum, &proxAddress);
	printHostInfo();
	portInfo(&proxAddress, proxSockFD); 
	


	while(1)
	{
		sentSegmentP *thisSegment = malloc (sizeof(sentSegmentP));
		recvfrom(proxSockFD, thisSegment, sizeof(sentSegmentP), 0, (struct sockaddr *)&senderAddress, &addr_size);
		printf("segment Message after recvFrom is: %s\n", thisSegment->segMessage);

		if(thisSegment->ack == 0)
			thisSegment->ack = 1;

		sendto(proxSockFD, thisSegment, sizeof(sentSegmentP), 0, (const struct sockaddr *)&senderAddress, sizeof(senderAddress));

		
		//just testing stuff out
		sentMessage(proxSockFD, thisSegment, rcvHostName, rcvPort);
		free(thisSegment);

	}


	
	return 0;
}
