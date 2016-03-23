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
 * Reads client message and responds accordingly
 */
int processInfo(char *buffer, char *rcvString);

/*
 * Handle incorrect or invalid formatting
 */
void unknownError(char *buffer, char *rcvString);

/*
 * Echo the message back to the client
 */
int echo(char rcvString[256]);

/*
 * Wrap the message in appropriate XML tags
 */
void changeEcho(char * buffer, char * rcvString);

/*
 * Gracefully shutdown the server
 */
int pShutdown(char rcvString[256]);

/*
 * Calculate the average server load
 */
int loadAvg(char rcvString[256]);

/*
 * Wrap the load avg in appropriate XML tags
 */
void changeLoadAvg(char * buffer, char * rcvString);

/* 
 * Get the current load on the server
 */
int getLoad(char *store);

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
SegmentP *createSegment(int i, char *parsedChars, SegmentP *thisSegment);

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
void handleTimerResult(int sockFD, struct sockaddr_in proxAddress, SegmentP *rcvSegment, int selectVal);

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
