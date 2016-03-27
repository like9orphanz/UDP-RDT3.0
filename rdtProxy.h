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
	int isCorrupt;
	int seqNum;
	char segMessage[10];
} sentSegmentP;

typedef struct thread
{
	int sockFD;
	char *destName;
	struct sockaddr *sender;
	socklen_t sender_size;
	int isDone;
	int portNumber;
	int destPort;
	int ack;
	int isCorrupt;
	int seqNum;
	char segMessage[10];
} threadP;

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
int isLostDelayedCorrupt(double lost, double delayed, double error, int duplicate);

/*
 * Appropriately handle the result of isLostDelayedCorrupt()
 */
void handleLDC(int LDC, sentSegmentP *thisSegment, int sockFD, char *rcvHostName, int rcvPort, struct sockaddr *senderAddress, socklen_t addr_size, int duplicate, int portNum);

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 */
void checkArgCount(int argc);

/*
 * Make sure LDC does not exceed 99%
 */
void checkLDCRange(int lost, int delayed, int corrupt);

#endif
