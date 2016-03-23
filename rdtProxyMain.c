/* * * * * * * * * */
/* rdtSenderMain.c */
/* * * * * * * * * */

/*     
 * Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * The main function which uses the functions from rdtSender.h
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
#include <time.h>

 int main(int argc, char *argv[])
{
	int portNum, rcvPort, sockFD, LDC;
	double lostPercent, delayedPercent, errorPercent;
	char *rcvHostName;
	struct sockaddr_in proxAddress; //local address
	// struct sockaddr_in rcvAddress; // sender's address
	struct sockaddr_in senderAddress;
	
	socklen_t addr_size = sizeof(senderAddress);
	srand(time(NULL));

 	if (argc != 7)
	{
		printf("Rerun rdtProxy with correct command line arguments\n");
		exit (-1);
	}

	rcvHostName = (char *) malloc(sizeof(char) * 1024);
	portNum = atoi(argv[1]); //local port
	strcpy(rcvHostName, argv[2]);
	rcvPort = atoi(argv[3]); 
	lostPercent = atof(argv[4]) * 100;
	delayedPercent = atof(argv[5]) * 100;
	errorPercent = atof(argv[6]) * 100;
	if (lostPercent + delayedPercent + errorPercent > 98)
	{
		printf("Every packet will be lost, delayed or corrupt, please rerun rdtProxy with values summing up to less than 100\n");
		exit(-1);
	}
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

		// 'Roll the di' to see if the network should 'lose', 'delay' or 'corrupt' packet
		LDC = isLostDelayedCorrupt(lostPercent, delayedPercent, errorPercent);
		
		// Delay packet (need to dispatch a thread)
		if (LDC == 2)
			;
		// 'Corrupt' packet
		if (LDC == 3)
			;
			
		// Always send so long as packet isn't 'lost'
		if (LDC != 1)
		{
			sendto(proxSockFD, thisSegment, sizeof(sentSegmentP), 0, (const struct sockaddr *)&senderAddress, sizeof(senderAddress));
			sentMessage(proxSockFD, thisSegment, rcvHostName, rcvPort);
		}
		// Request sender to resend 'lost' packet by forcing timeout
		else
		{
			printf("puttin her to sleep\n");
			sleep(6);	
		}
		free(thisSegment);
	}

	return 0;
}
