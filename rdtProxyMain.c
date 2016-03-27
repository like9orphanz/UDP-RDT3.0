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
	int portNum, rcvPort, LDC = 0, proxSockFD, duplicate = 0;
	double lostPercent, delayedPercent, errorPercent;
	char *rcvHostName;
	struct sockaddr_in proxAddress, senderAddress;
	
	checkArgCount(argc);

	socklen_t addr_size = sizeof(senderAddress);
	rcvHostName = (char *) malloc(sizeof(char) * 1024);
	portNum = atoi(argv[1]); //local port
	strcpy(rcvHostName, argv[2]);
	rcvPort = atoi(argv[3]); 
	lostPercent = atoi(argv[4]);
	delayedPercent = atoi(argv[5]);
	errorPercent = atoi(argv[6]);
	
	checkLDCRange(lostPercent, delayedPercent, errorPercent);
	srand(time(NULL));
	
	proxSockFD = sockCreation(rcvHostName, portNum, &proxAddress);
	printHostInfo();
	portInfo(&proxAddress, proxSockFD); 

	while(1)
	{	
		sentSegmentP *thisSegment = malloc (sizeof(sentSegmentP));
		//Receive segment from Sender
		if (duplicate == 0)
		{
			recvfrom(proxSockFD, thisSegment, sizeof(sentSegmentP), 0, (struct sockaddr *)&senderAddress, &addr_size);
			printf("Sender->segMessage: %s\n", thisSegment->segMessage);
			if (LDC == 2)
				duplicate = 1;
		}
		// 'Roll the di' to see if the network should 'lose', 'delay' or 'corrupt' packet
		LDC = isLostDelayedCorrupt(lostPercent, delayedPercent, errorPercent, duplicate);
		handleLDC(LDC, thisSegment, proxSockFD, rcvHostName, rcvPort, (struct sockaddr *)&senderAddress, addr_size, duplicate, portNum);
		if (duplicate == 1) 
			duplicate = 0;
	}
	return 0;
}
