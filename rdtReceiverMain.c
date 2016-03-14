#include "rdtReceiver.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	int portNum, sockFD;

	if (argc != 2)
	{
		printf("Rerun with port number as command line argument\n");
		exit(-1);
	}

	portNum = atoi(argv[1]);
	sockFD = createSocket();
	printHostInfo();

	return 0;
}