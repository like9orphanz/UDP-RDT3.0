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
#include "rdtSender.h" 
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
	int portNum, rcvPort, sockFD;
	double lostPercent, delayedPercent, errorPercent;
	char *rcvHostName;
	struct sockaddr_in proxAddress; //local address
	struct sockaddr_in rcvAddress; // sender's address
	struct sockaddr_in senderAddress;
	socklen_t addr_size = sizeof(senderAddress);
	sentSegmentP thisSentSegment = malloc(sizeof(struct sentSegment));
/*
 	if (argc != 7)
	{
		printf("Rerun rdtProxy with correct command line arguments\n");
		exit (-1);
	}
*/
	rcvHostName = (char *) malloc(sizeof(char) * 1024);
	
	//Assign all command line arguments to their respective variables
	portNum = atoi(argv[1]); //local port
	strcpy(rcvHostName, argv[2]);
	rcvPort = atoi(argv[3]); 
	//lostPercent = atoi(argv[4]);
	//delayedPercent = atoi(argv[5]);
	//errorPercent = atoi(argv[6]);
	
	/*sockFD = sockCreation(rcvHostName, rcvPort, &rcvAddress);
	int proxSockFD = sockCreation(rcvHostName, portNum, &proxAddress);
	printHostInfo();
	portInfo(&proxAddress, proxSockFD); 
	*/

///// This is going to be a test run ///////////////

	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	memset(&proxAddress, 0, sizeof(proxAddress));
	proxAddress.sin_family = AF_INET;
	proxAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	proxAddress.sin_port = htons(portNum);

	bind(sock, (struct sockaddr *) &proxAddress, sizeof(proxAddress));

	
//// END OF TEST///////////////////
	while(1)
	{
		recvfrom(sock, thisSentSegment, sizeof(thisSentSegment), 0, (struct sockaddr*)&rcvAddress, &addr_size);

		printf("segment Message is: %s\n", thisSentSegment->segMessage);
	}
		
	
	return 0;
}
