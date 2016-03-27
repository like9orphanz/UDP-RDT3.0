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

#include "rdtSender.h"
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
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{	
	int portNum, proxyPortNum, sockFD, selVal, corrupt, runs, i = 0;
	char *proxyHostName, *inputMessage;
	struct sockaddr_in proxAddress, sendAddress;
	checkArgCount(argc);
	
	proxyHostName = (char *) malloc (sizeof(char) * 1024);
	portNum = atoi(argv[1]);
	strcpy(proxyHostName, argv[2]);
	proxyPortNum = atoi(argv[3]);
	sockFD = createSocket();
	int sendSockFD = sockCreation(proxyHostName, portNum, &sendAddress);
    	printHostInfo();
    	portInfo(&sendAddress, sendSockFD);
	inputMessage = getMessage();	
	
	if ((strlen(inputMessage)%2) == 0)
		runs = strlen(inputMessage) / 4;
	else
		runs = (strlen(inputMessage) / 4) + 1;
	
	while (i < runs)
	{	
		// Send segment
		SegmentP *sendSegment = createSegment(i, (parseMessage(i, inputMessage)));
		sendMessage(sockFD, sendSegment, proxyHostName, proxyPortNum);
		
		// Wait on ack, maximum of 5 seconds
		SegmentP *rcvSegment = malloc(sizeof(SegmentP));
		selVal = runTimer(sockFD);
		corrupt = handleTimerResult(sockFD, &proxAddress, rcvSegment, sendSegment, proxyHostName, proxyPortNum, selVal);
		if (selVal == 1 && corrupt == 0) i++;
		
		free(sendSegment);
		free(rcvSegment); 
	}
	return 0;
}
