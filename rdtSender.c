/* 
 * UDPserver.c 
 *     
 * Sam Stein
 * Joshua Wright
 *
 * Systems and Networks 2
 * Project 4
 */

#include "rdtSender.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>


/*
 *	This program is to simulate a server that can communicate with a client and 
 *	respond accordingly. Depending on what message the client sends the server will 
 *	send either <reply></reply>, the load average, or <error></error>
 *
 */
int ListenSockCreation(int port, struct sockaddr_in *address);

void handler(int param)
{
	fprintf(stderr, "\nCTRL-C has been entered. The server is shut down.\n");
	exit(EXIT_SUCCESS);
}
void printHostInfo()
{
 	char hostname[1024];
        hostname[1023] = '\0';
	struct hostent * hostptr;
	gethostname(hostname, 1023);
	//find the ip address
	hostptr = gethostbyname(hostname);
	fprintf(stderr, "Host Name: %s\n", hostname);
	fprintf(stderr, "IP address: %s\n", inet_ntoa(*(struct in_addr*)hostptr->h_addr));

}
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
	struct sockaddr_in printSock;
	socklen_t addrLen = sizeof(struct sockaddr);
        getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
	fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}
int processInfo(char *buffer, char *rcvString);
void unknownError(char *buffer, char *rcvString);
int echo(char rcvString[256]);
void changeEcho(char * buffer, char * rcvString);
int pShutdown(char rcvString[256]);
int loadAvg(char rcvString[256]);
void changeLoadAvg(char * buffer, char * rcvString);
int getLoad(char *store);


int main(int argc, char** argv)
{
	if(argc != 2)
        {
                fprintf(stderr, "Please input <port>\n");
                exit(1);
        }

	//Listen socket descriptor (reference)
	int ls;
	//Number of bytes to send or reciever
	int len = 0;
	//Size of waiting clients
	int waitSize = 16;
	//Server address, and client address
	struct sockaddr_in serverAddress, clientAddress;
	struct in_addr a;
	socklen_t addr_size;
	addr_size = sizeof(clientAddress);
	char sentMessage[256];
	int errorCheck = 0;

	//bind the socket
	ls = ListenSockCreation(atoi(argv[1]), &serverAddress);

	printHostInfo();
	portInfo(&serverAddress, ls);

	char buffer[256];
	bzero(buffer, 256);
	
	//infinite loop, accept connection and process info
	while(1)
	{
		len = recvfrom(ls, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &addr_size);

		printf("Connected with %s at port %d\n", inet_ntoa(clientAddress.sin_addr), htons(clientAddress.sin_port));
		if(len < 0)
			fprintf(stderr, "ERROR in recvfrom\n");

		if((processInfo(buffer, sentMessage)) == 0)
			sendto(ls, sentMessage, sizeof(sentMessage), 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr_in));
		else
		{
			strcpy(sentMessage, "Shutting Down");
			sendto(ls, sentMessage, sizeof(sentMessage), 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr_in));
			return(EXIT_SUCCESS);
		}
	}

	return(EXIT_SUCCESS);
}

/*
 *	processInfo : reads the message that the client sends and depending on what the message says 
 *		responds accordingly
 *
 */
int processInfo(char *buffer, char *rcvString)
{
	int errorCheck = 0, response = 0;
	int length = strlen(buffer);
	if(length > 256)
		return response;
	bzero(rcvString, 256);
	if((errorCheck = pShutdown(buffer)) == 0)
		response = 1;
	else if((errorCheck = echo(buffer)) != 0)
		changeEcho(buffer, rcvString);
	else if((errorCheck = loadAvg(buffer)) != 0)
		changeLoadAvg(buffer, rcvString);
	else
		unknownError(buffer, rcvString);
	
	return response;


}

void unknownError(char *buffer, char *rcvString)
{
	bzero(rcvString, 256);
	strcpy(rcvString, "<error>unknown format</error>");
}

int echo(char * rcvString)
{
	int response = 0;
	char dest[256];
	bzero(dest, 256);
	//puts the first 6 chars in the dest array
	strxfrm(dest, rcvString, 6);
	char *end = strstr(rcvString, "</echo>");
        if(end != NULL)
	{
		if((strcmp(dest, "<echo>") == 0) && (strcmp(end, "</echo>") == 0))
			response = 1;
	}
	return response;


}
void changeEcho(char * buffer, char * rcvString)
{
	int i = 0, length = 0;
	char reply[256];
	char *tp;
	char store[256];
	bzero(rcvString, 256);
	bzero(store, 256);
	bzero(reply, 256);
	strcpy(rcvString, buffer);
	tp = strstr(rcvString, ">");
	tp = tp + sizeof(char);
	memmove(rcvString, tp, 256);
	length = strlen(rcvString);
	char temp = '\0';
	while(i < length)
	{
		temp = rcvString[i];
		if(temp == '<')
			i = length;
		else
			store[i] = temp;
		i++;
	}
	strcpy(rcvString, store);
	strcpy(reply, "<reply>");
	strcat(reply, rcvString);
	strcat(reply, "</reply>");
	bzero(rcvString, 256);
	strcpy(rcvString, reply);
	

}
int pShutdown(char * rcvString)
{
	int response = 1;
	if((strcmp(rcvString, "<shutdown/>")) == 0)
		response = 0;
	return response;

}

int loadAvg(char rcvString[256])
{
	if((strcmp(rcvString, "<loadavg/>")) == 0)
		return 1;
	else
		return 0;

}

void changeLoadAvg(char * buffer, char * rcvString)
{
	char store[256];
	bzero(store, 256);
	strcpy(rcvString, "<replyLoadAvg>");
	int errorCheck = getLoad(store);
	if(errorCheck == -1)
	{
		fprintf(stderr, "Failed getting load aver\n");
		exit(-1);
	}
	strcat(rcvString, store);
	strcat(rcvString, "</replyLoadAvg>");
}

int getLoad(char *store)
{
	double loadAvg[3];
	bzero(store, 256);
	int errorCheck = 0;
	errorCheck = getloadavg(loadAvg, 3);
	if(errorCheck != -1)
	{
		sprintf(store, "%f:%f:%f", loadAvg[0], loadAvg[1], loadAvg[2]);
	}
	return errorCheck;

}
//function to creation listen socket and bind it
int ListenSockCreation(int port, struct sockaddr_in *address)
{
	int sock_ls;

        memset(address, 0, sizeof(*address));
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons(port);

	//creates a socket
	if((sock_ls = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
                fprintf(stderr, "Error: listen sock failed!");
                exit(1);
        }

	//binds the socket
        if(bind(sock_ls, (struct sockaddr *)address, sizeof(*address)) < 0)
        {
                fprintf(stderr, "Error binding\n");
                close(sock_ls);
                exit(1);
        }

	return sock_ls;

}					
