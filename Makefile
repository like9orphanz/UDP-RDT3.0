CFLAGS = -g -Wall -lm
CC = gcc

all : rdtSender rdtReceiver rdtProxy

rdtSender : rdtSender.c rdtSender.h rdtSenderMain.c
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSenderMain.c

rdtReceiver : rdtReceiver.c rdtReceiver.h rdtReceiverMain.c
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiverMain.c

rdtProxy : rdtProxy.c rdtProxy.h rdtProxyMain.c
	gcc $(CFLAGS) -o rdtProxy rdtProxy.c rdtProxyMain.c

clean :
	rm rdtSender rdtReceiver rdtProxy
