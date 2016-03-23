/* * * * * * * */
/* rdtProxy.h  */
/* * * * * * * */    

/* Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * This file describes the functions to be implemented by an RDT sender.
 */

#ifndef _RDTPROXY_H
#define _RDTPROXY_H

#include <netinet/in.h>

typedef struct sentSegment
{
	int ack;
	int seqNum;
	int messageSize;
	char segMessage[10];
} sentSegmentP;

/*
 * Create and bind the socket
 */
int sockCreation(char *hostName, int port, struct sockaddr_in *dest);

/*
 * Print the host information to the terminal
 */
void printHostInfo();

/*
 * Display the port information to the screen
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd);

/*
 * Creates a datagram socket, returns the socket file descriptor
 */
int createSocket();

/*
 * Send message to receiver
 */
int sentMessage(int sockFD, sentSegmentP *thisSegment, char * serverName, int serverPort);

/*
 * Roll dice' to see if segment is lost, delayed or corrupt
 *
 * Paramters :
 * pLost    - User entered percentage lost
 * pDelayed - User entered perecentage delayed
 * pError   - User entered percentage error (corrupt packet)
 * 
 * Returns :
 * 0 - Neither lost, delayed or an error in packet transmission
 * 1 - Packet is lost
 * 2 - Packet is delayed
 * 3 - Error with packet (corrupt)
 */
int isLostDelayedCorrupt(double lost, double delayed, double error);

#endif
