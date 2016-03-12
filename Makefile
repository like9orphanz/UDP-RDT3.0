CFLAGS = -g -Wall
CC = gcc

all : rdtSender rdtReceiver

rdtSender : rdtSender.c rdtSender.h
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSender.h

rdtReceiver : rdtReceiver.c rdtReceiver.h
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiver.h rdtReceiverMain.c

clean :
	rm rdtSender rdtReceiver
