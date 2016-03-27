# UDP-RDT3.0

Sam Stein
Joshua Wright
Systems and Networks 2
Project 4

This project implements RDT3.0 over UDP by allowing the user to input parameters such as delayed, lost and errors in packet transmission via the command line.

--Sample input to the command line --

--Program 1 --
rdtReceiver
	./rdtReceiver 60050
	hostname - cs-ssh1.cs.uwf.edu
	port - 60050

--Program 2 --
rdtProxy
	./rdtProxy 60051 cs-ssh1.cs.uwf.edu 60050 20 40 20
	hostname - cs-ssh2.cs.uwf.edu
	port - 60051

--Program 3 --
rdtSender
	./rdtSender 60052 cs-ssh1.cs.uwf.edu 60051

our payload is a segmented message of 4 bytes
our header is 3 ints (ack, isCorrupt, seqNum) totalling 6 bytes
payload and header combined into 1 data structure to be sent is 10 bytes as instructed.
