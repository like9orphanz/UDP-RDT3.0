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
	int portNum, proxyPortNum, sockFD, i = 0;
	char *proxyHostName;
	struct sockaddr_in proxAddress, sendAddress;
	char inputMessage[256];
	time_t start, end;

	socklen_t addr_size = sizeof(proxAddress);
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
	while (i < (strlen(inputMessage) / 4))
	{	
		// Send segment
		SegmentP *sendSegment = malloc(sizeof(SegmentP));
		sendSegment = createSegment(i, (parseMessage(i, inputMessage)), sendSegment);
		printf("segment Message = %s\n", sendSegment->segMessage);
		sendMessage(sockFD, sendSegment, proxyHostName, proxyPortNum);
		
		// Wait on ack, maximum of three seconds
		SegmentP *rcvSegment = malloc(sizeof(SegmentP));
		printf("ack before i recvfrom = %d\n", rcvSegment->ack);
		/*
		 * THIS IS THE PROBLEM, CANT TIME THE TIME TO RECIEVE BECAUSE
		 * IM MEASURING TIME AFTER recvfrom() BUT THE PROCESS HANGS BECAUSE
		 * recvfrom() IS A BLOCKING CALL
		 */
		time(&start);
		recvfrom(sockFD, rcvSegment, sizeof(SegmentP), 0, (struct sockaddr *)&proxAddress, &addr_size);
		time(&end);
		if (difftime(end, start) <= 3)
		{
			printf("proxy sent back ack\n");
			printf("ack after i recvfrom = %d\n", rcvSegment->ack);
			i++;
		}
		else
			printf("Ack wait timed out, resending packet\n");

		free(sendSegment);
		free(rcvSegment); 
	}

	return 0;
}
