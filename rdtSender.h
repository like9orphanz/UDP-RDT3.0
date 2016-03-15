/*
 * rdtSender.h
 * Systems and Networks II
 * Project 4
 *
 * This file describes the function(s) to be implemented by an RDT sender.
 */

<<<<<<< HEAD
#ifndef _RDT_SENDER_H
#define _RDT_SENDER_H

=======
#ifndef _RDTSENDER_H
#define _RDTSENDER_H
#include <netinet/in.h>
>>>>>>> Proxy-Basic
/*
 * Sends a message to an RDT receiver on a specified host and port.
 * 
 * destHost  - the name of the host on which the receiver runs
 * destPort  - the number of the port on which the receiver listens
 * message   - the entire null terminated message to be sent to the receiver
 *
 * return 0, if no error; otherwise, a negative number indicating the error
 */
int sendMessage (char* desthost, int destPort, char* message);

<<<<<<< HEAD
#endif _RDT_SEND_H
=======
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
 * Creates the listening socket
 */
int ListenSockCreation(int port, struct sockaddr_in *address);

/*
 * Prints the host information to the terminal
 */
void printHostInfo();

/*
 * Handle the shut down signal
 */
void handler(int param);

#endif 
>>>>>>> Proxy-Basic
