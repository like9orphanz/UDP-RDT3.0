/*
 * rdtReceiver.h
 * Systems and Networks II
 * Project 4
 *
 * This file describes the functions to be implemented by an RDT receiver.
 */

 #ifndef _RDT_RECEIVER_H
 #define _RDT_RECEIVER_H

/*
 * Receives a message from an RDT sender on a specified port.
 *
 * port - the number of the port on the host that receives messages from the sender
 *
 * returns the text message received by a sender or NULL if an error occurred
 */
char* receiveMessage (int port);


#endif