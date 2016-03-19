#ifndef _RDTPROXY_H
#define _RDTPROXY_H



typedef struct sentSegment
{
	int ack;
	int seqNum;
	int messageSize;
	char segMessage[10];
}sentSegmentP;



/*
 * Print the host information to the terminal
 */
void printHostInfo();

/*
 * Creates a datagram socket, returns the socket file descriptor
 */
int createSocket();

int sentMessage(int sockFD, sentSegmentP *thisSegment, char * serverName, int serverPort);
#endif
