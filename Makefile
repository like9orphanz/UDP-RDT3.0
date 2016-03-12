CFLAGS = -g -Wall
CC = gcc

all : rdtSender rdtReceiver

rdtSender : rdtSender.c rdtSender.h
	gcc $(CFLAGS) -o rdtSender rdtSender.c

rdtReceiver : rdtReceiver.c  rdtReceiverMain.c rdtReceiver.h
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiverMain.c

clean :
	rm rdtSender rdtReceiver
