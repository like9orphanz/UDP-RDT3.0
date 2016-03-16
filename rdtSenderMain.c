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

int main(int argc, char *argv[])
{	
	int portNum, proxyPortNum;
	char *proxyHostName, *message;

	int i = 0;
	
	if (argc != 4)
	{
		printf("Rerun rdtSender with the correct command line arguments\n");
		exit(-1);
	}
	
	proxyHostName = (char *) malloc (sizeof(char) * 1024);

	portNum = atoi(argv[1]);
	strcpy(proxyHostName, argv[2]);
	proxyPortNum = atoi(argv[3]);

	message = getUserInput();

	// Parse message into segments
	while (i < strlen(message) - 1)
	{	
		printf("in while\n");
		SegmentP thisSegment = createSegment(parseMessage(i, message));
		if (thisSegment == 0x00) break;
		printf("thisSegment->segMessage @ %d = %s\n", i, thisSegment->segMessage);
		free(thisSegment);
		i++;
	}

	return 0;
}