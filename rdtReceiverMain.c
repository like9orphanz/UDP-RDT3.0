/* * * * * * * * * * */
/* rdtReceiverMain.c */
/* * * * * * * * * * */

/*     
 * Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 * UDP-RDT 3.0
 *
 * The main function which uses the functions from rdtReceiver.h
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

int main(int argc, char *argv[])
{
	int sockFD, duplicate, prevAck = 2;
	struct sockaddr_in rcvAddress, sendMessage;
	socklen_t addr_size = sizeof(sendMessage);
	char printMessage[256];
	bzero(printMessage, 256);
	if (argc != 2)
	{
		printf("Rerun with port number as command line argument\n");
		exit(-1);
	}

	sockFD = sockCreation(atoi(argv[1]), &rcvAddress);
	printHostInfo();
	portInfo(&rcvAddress, sockFD);

	while (1)
	{
		recvSegmentP *thisSegment = malloc(sizeof(recvSegmentP));
		recvfrom(sockFD, thisSegment, sizeof(recvSegmentP), 0, (struct sockaddr *)&sendMessage, &addr_size);
		duplicate = isDuplicateSegment(thisSegment, prevAck);
		// Accept segment into final message
		if (thisSegment->isCorrupt == 0 && duplicate != 1)
		{
			strcat(printMessage, thisSegment->segMessage);
			printf("Message Forming: %s\n", printMessage);

			prevAck = thisSegment->ack;

			if (thisSegment->ack == 1)
				thisSegment->ack = 0;
			else 
				thisSegment->ack = 1;
			
			sendto(sockFD, thisSegment, sizeof(recvSegmentP), 0, (struct sockaddr *)&sendMessage, addr_size);
		}
		if (thisSegment->isCorrupt == 1 && duplicate != 1)
		{
			sendto(sockFD, thisSegment, sizeof(recvSegmentP), 0, (struct sockaddr *)&sendMessage, addr_size);
		}

		free(thisSegment);
	}
	return 0;
}
