/*
 * rdtSender.h
 * Systems and Networks II
 * Project 4
 *
 * This file describes the function(s) to be implemented by an RDT sender.
 */

#ifndef _RDT_SENDER_H
#define _RDT_SENDER_H

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

#endif _RDT_SEND_H