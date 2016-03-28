/* * * * * * * */
/* rdtSender.h */
/* * * * * * * */    

/* Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * This file describes the functions to be implemented by an RDT sender.
 */

#ifndef _RDTSENDER_H
#define _RDTSENDER_H

#include <netinet/in.h>

typedef struct Segment
{
	int ack;
	int isCorrupt;
	int seqNum;
	char segMessage[10];
} SegmentP;

/*
 * Send message to the proxy
 *
 * sockFD      - the file descriptor
 * thisSegment - the struct containing the header and payload
 * serverName  - the name of the proxy
 * serverPort  - the port for the proxy 
 */ 
int sendMessage(int sockFD, SegmentP *thisSegment, char * serverName, int serverPort);

/*
 * Displays port info with the getsockname() funciton
 *
 * serverAddress  - the struct containing the address info
 * sockfd         - file descriptor
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd);

/*
 * Create socket
 */
int createSocket();

/*
 * Create the socket
 *
 * hostName - the name of the host
 * port     - the port of the host
 * dest     - struct containing the address info    
 */
int sockCreation(char * hostName, int port, struct sockaddr_in *dest);


/*
 * Prints the host information to the terminal
 */
void printHostInfo();

/*
 * Handle the shut down signal
 */
void handler(int param);

/*
 * Create a 'segment' structure, assign the pased string to segMessage
 *
 * i            - the count
 * parsedChars  - pointer to the segment
 */
SegmentP *createSegment(int i, char *parsedChars);

/*
 * Parse the message obtained from user
 *
 * count   - counter
 * message - containing the message input by the user
 */
char *parseMessage(int count, char *message);

/*
 * Detects a timeout in receiving an ack from the receiver
 */
int runTimer(int sockFD);

/*
 * Appropriatly handles any valid output from runTimer()
 *
 * sockFD      - the file descriptor
 * proxAddress - struct containing the proxy info
 * rcvSegment  - the struct containing the recv payload and header
 * thisSegment - the struct containing the sent payload and header
 * serverName  - the name of the host
 * serverPort  - the port Number of the host
 * selectVal   - the value of the select function from the sender 
 */
int handleTimerResult(int sockFD, struct sockaddr_in *proxAddress, SegmentP *rcvSegment, SegmentP *thisSegment, char * serverName, int serverPort, int selectVal);

/*
 * Make sure the number of command line parameters entered
 * by the user is correct
 */
void checkArgCount(int argc);

/*
 * Get the message to be sent to receiver from the user 
 */
char *getMessage();

#endif 
