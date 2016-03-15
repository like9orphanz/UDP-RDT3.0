CFLAGS = -g -Wall
CC = gcc

all : rdtSender rdtReceiver rdtProxy

rdtSender : rdtSender.c rdtSender.h
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSender.h

rdtReceiver : rdtReceiver.c rdtReceiver.h
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiver.h

rdtProxy : rdtProxy.c rdtProxy.h rdtProxyMain.c
	gcc $(CFLAGS) -o rdtProxy rdtProxy.c rdtProxyMain.c

clean :
	rm rdtSender rdtReceiver rdtProxy
