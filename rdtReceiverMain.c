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
	int sockFD;
	struct sockaddr_in rcvAddress;
	if (argc != 2)
	{
		printf("Rerun with port number as command line argument\n");
		exit(-1);
	}

	sockFD = sockCreation(atoi(argv[1]), &rcvAddress);
	printHostInfo();
	portInfo(&rcvAddress, sockFD);

	while(1)
	{
		recvSegmentP *thisSegment = malloc(sizeof(recvSegmentP));
		receiveResponse(sockFD, thisSegment);
		printf("recvSegment = %s\n", thisSegment->segMessage);
		free(thisSegment);
	}
	return 0;
}
