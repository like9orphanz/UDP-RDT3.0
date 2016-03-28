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
 *
 * hostName  - the name of the Host
 * port  - the port number
 * dest  - struct containing the address info
 */
int sockCreation(char *hostName, int port, struct sockaddr_in *dest);

/*
 * Display the port information to the screen
 *
 * serverAddress  - struct containing the address info
 * sockfd  - file descriptor
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
 *
 * sockFD  - file descriptor
 * thisSegment  - payload and header
 * serverName  - address struct for receiver
 * serverPort  - port number of the receiver         
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
 *
 * LCD           - contains either 0 1 2 or 3
 * thisSegment   - payload and header
 * sockFD        - file descriptor
 * rcvHostName   - name of the receiver hostname
 * rcvPort       - port number of the receiver
 * senderAddress - struct of the sender address
 * addr_size     - size of the sender address
 * duplicate     - either 1 or 0 for duplicates
 * portNum       - holds the prox port number
 */void handleLDC(int LDC, sentSegmentP *thisSegment, int sockFD, char *rcvHostName, int rcvPort, struct sockaddr *senderAddress, socklen_t addr_size, int duplicate, int portNum);

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 *
 * argc  - the amount of the arguments from the command line
 */
void checkArgCount(int argc);

/*
 * Make sure LDC does not exceed 99%
 *
 * lost  - count for lost packets
 * delayed  - count for delayed packets
 * corrput  - count for corrupt packets
 */
void checkLDCRange(int lost, int delayed, int corrupt);

#endif
