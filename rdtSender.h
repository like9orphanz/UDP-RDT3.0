  /*
 * rdtSender.h
 * Systems and Networks II
 * Project 4
 *
 * This file describes the function(s) to be implemented by an RDT sender.
 */

#ifndef _RDTSENDER_H
#define _RDTSENDER_H
#include <netinet/in.h>
/*
 * Sends a message to an RDT receiver on a specified host and port.
 * 
 * destHost  - the name of the host on which the receiver runs
 * destPort  - the number of the port on which the receiver listens
 * message   - the entire text message to be sent to the receiver; the message is \0 terminated
 *
 * return 0, if no error; otherwise, a negative number indicating the error
 */
int sendMessage (char* desthost, int destPort, char* message);
int processInfo(char *buffer, char *rcvString);
void unknownError(char *buffer, char *rcvString);
int echo(char rcvString[256]);
void changeEcho(char * buffer, char * rcvString);
int pShutdown(char rcvString[256]);
int loadAvg(char rcvString[256]);
void changeLoadAvg(char * buffer, char * rcvString);
int getLoad(char *store);
void portInfo(struct sockaddr_in *serverAddress, int sockfd);
int ListenSockCreation(int port, struct sockaddr_in *address);
void printHostInfo();
void handler(int param);

#endif 