/* * * * * * * * */
/* rdtReceiver.h */
/* * * * * * * * */    

/* Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 *
 * This file describes the functions to be implemented by an RDT receiver.
 */

 #ifndef _RDT_RECEIVER_H
 #define _RDT_RECEIVER_H

#include <netinet/in.h>

typedef struct Segment
{
	int ack;
	int isCorrupt;
	int seqNum;
	char segMessage[10];
} recvSegmentP;


/*
 * Receives a message from an RDT sender on a specified port.
 *
 * port - the number of the port on the host that receives messages from the sender
 *
 * returns the text message received by a sender or NULL if an error occurred
 */
char* receiveMessage (int port);
/*
 * Creates a datagram socket.
 *
 * return value - the socket identifier or a negative number indicating the error for creating a socket
 */
int createSocket();

/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sockFD     - the socket identifier
 * request    - the request to be sent encoded as a string
 * serverName - the ip address or hostname of the server given as a string
 * serverPort - the port number of the server
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sockFD, char * request, char * serverName, int serverPort);

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sockfd    - the socket identifier
 * response  - the server's response as an XML formatted string to be filled in by this function into the specified string array
 * size      - the size of the buffer for storing a response from the server
 * 
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sockFD, recvSegmentP *thisSegment);

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char * response);

/*
 * Closes the specified socket
 *
 * sockFD - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sockFD);

/*
 * Print host information to the screen
 */
void printHostInfo();

/*
 * Create and bind socket
 */
void portInfo(struct sockaddr_in *serverAddress, int sockfd);

/*
 * Display the port information
 */
int sockCreation(int port, struct sockaddr_in *address);

#endif
