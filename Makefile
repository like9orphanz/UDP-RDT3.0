CFLAGS = -g -Wall
CC = gcc

all : rdtSender rdtReceiver rdtProxy

<<<<<<< HEAD
<<<<<<< HEAD
rdtSender : rdtSender.c rdtSender.h
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSender.h
=======
rdtSender : rdtSender.c rdtSenderMain.c rdtSender.h 
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSenderMain.c
>>>>>>> Proxy-Basic
=======
rdtSender : rdtSender.c rdtSenderMain.c rdtSender.h 
	gcc $(CFLAGS) -o rdtSender rdtSender.c rdtSenderMain.c
>>>>>>> Proxy-Basic

rdtReceiver : rdtReceiver.c rdtReceiver.h
	gcc $(CFLAGS) -o rdtReceiver rdtReceiver.c rdtReceiver.h

rdtProxy : rdtProxy.c rdtProxy.h rdtProxyMain.c
	gcc $(CFLAGS) -o rdtProxy rdtProxy.c rdtProxyMain.c

rdtProxy : rdtProxy.c rdtProxy.h rdtProxyMain.c
	gcc $(CFLAGS) -o rdtProxy rdtProxy.c rdtProxyMain.c

clean :
	rm rdtSender rdtReceiver rdtProxy
