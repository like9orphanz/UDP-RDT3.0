/*
 * RDT-UDP3.0
 * rdtProxyMain.c
 *
 * Sam Stein
 * Joshua Wright
 *
 * Uses rdtProxy.c to pass messages from server to client 
 */

 #include "rdtProxy.h"
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>

 int main(int argc, char *argv[])
{
	int portNum, rcvPort, sockFD;
	double lostPercent, delayedPercent, errorPercent;
	char *rcvHostName;

	if (argc != 7)
	{
		printf("Rerun rdtProxy with correct command line arguments\n");
		exit (-1);
	}
	rcvHostName = (char *) malloc(sizeof(char) * 1024);
	
	//Assign all command line arguments to their respective variables
	portNum = atoi(argv[1]);
	strcpy(rcvHostName, argv[2]);
	rcvPort = atoi(argv[3]);
	lostPercent = atoi(argv[4]);
	delayedPercent = atoi(argv[5]);
	errorPercent = atoi(argv[6]);

	printHostInfo();

	sockFD = createSocket();

	return 0;
}