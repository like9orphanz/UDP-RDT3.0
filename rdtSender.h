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
	int seqNum;
	int messageSize;
	char segMessage[10];
} SegmentP;

/*
 * Sends a message to an RDT receiver on a specified host and port.
 * 
 * destHost  - the name of the host on which the receiver runs
 * destPort  - the number of the port on which the receiver listens
 * message   - the entire null terminated message to be sent to the receiver
 *
 * return 0, if no error; otherwise, a negative number indicating the error
 */
int sendMessage(int sockFD, SegmentP *thisSegment, char * serverName, int serverPort);

/*
 * Prints out the socket port with getsockname()
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd);

/*
 * Create socket
 */
int createSocket();

/*
 * Creates the listening socket
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
 */
SegmentP *createSegment(int i, char *parsedChars);

/*
 * Parse the message obtained from user in 'getUserInput()'
 */
char *parseMessage(int count, char *message);

/*
 * Detects a timeout in receiving an ack from the receiver
 */
int runTimer(int sockFD);

/*
 * Appropriatly handles any valid output from runTimer()
 */
void handleTimerResult(int sockFD, struct sockaddr_in proxAddress, SegmentP *rcvSegment, SegmentP *thisSegment, char * serverName, int serverPort, int selectVal);
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
