CFLAGS = -g -Wall
CC = gcc

all : rdtSender rdtReceiver rdtProxy

rdtSender : rdtSender.c rdtSenderMain.c rdtSender.h 
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSenderMain.c

rdtReceiver : rdtReceiver.c  rdtReceiverMain.c rdtReceiver.h
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiverMain.c

rdtProxy : rdtProxy.c rdtProxy.h rdtProxyMain.c
	gcc $(CFLAGS) -o rdtProxy rdtProxy.c rdtProxyMain.c

clean :
	rm rdtSender rdtReceiver rdtProxy
