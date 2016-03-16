#ifndef _RDTPROXY_H
#define _RDTPROXY_H

/*
 * Print the host information to the terminal
 */
void printHostInfo();

/*
 * Creates a datagram socket, returns the socket file descriptor
 */
int createSocket();

#endif