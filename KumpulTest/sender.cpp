#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utility.h"
#include "send_window.h"
#include <unistd.h>
#include <iostream>

using namespace std;

void die(char* errorMessage) {
	perror(errorMessage);
	exit(0);
}

int main(int argc, char* argv[]) {
	char* fileName = argv[1];
	unsigned int windowSize = atoi(argv[2]);
	unsigned int bufferSize = atoi(argv[3]);
	char* destIP = argv[4];
	int port = atoi(argv[5]);


	// Urusan Socket
	int udpSocket;
	struct sockaddr_in clientAddress;

	unsigned char buffer[bufferSize];
	
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		exit(0);
	}

	if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		die("socket");
	}

	//Urusan Timeout
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

	//Urusan Address
	memset((char *)&clientAddress, 0, sizeof(clientAddress));
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(port);
	clientAddress.sin_addr.s_addr = inet_addr(destIP);

	unsigned int slen = sizeof(clientAddress);
	unsigned int i = 0;

	//Urusan Segment dan Pengiriman
	Segment paket;
	PacketACK ack;
	unsigned int counterBuffer = 0;
	unsigned int counterSequence = 0;
	int alreadyReadAll = 0;
	unsigned int advertisedWindowSize = 256;
	unsigned int bufferSizeOffset = 0;

	unsigned char c;

	//Urusan window
	SendingWindow sendingWindow;
	createNew(&sendingWindow, windowSize);

	unsigned int repeatEnd, repeatStart;
	unsigned int paramSend = 0;
	unsigned int iterator = 0;

	srand(time(NULL));

	while (1) {
		printf("Sending Data: \n");
		
		//Urusan Buffer	
		while (counterBuffer < bufferSize && !alreadyReadAll) {
			if (fscanf(file, "%c", &c) == EOF) {
				alreadyReadAll = 1;
				break;
			}
			buffer[counterBuffer] = c;
			printf("Read Data : %d %c \n", counterBuffer + bufferSizeOffset, buffer[counterBuffer]);
			counterBuffer++;
		}

		//Sending Frame

		// printf("BUFF %d OFFSET %d\n", bufferSize, bufferSizeOffset);
		// printf("LAR %d LFS %d\n", LAR(sendingWindow), LFS(sendingWindow));
		repeatStart = LAR(sendingWindow);
		repeatEnd = LFS(sendingWindow);
		
		paramSend = 0;
		while (LFS(sendingWindow) < LAR(sendingWindow) + windowSize && LFS(sendingWindow) < counterBuffer + bufferSizeOffset && paramSend < advertisedWindowSize) {

			//Create Segment
			paket = CreateSegment(LFS(sendingWindow), buffer[LFS(sendingWindow) - bufferSizeOffset], 0);
			Checksum(paket) = generateChecksumPaket(paket);
			
			LFS(sendingWindow) = LFS(sendingWindow) + 1;

			printf("Frame Number %d : DATA %c\n", LFS(sendingWindow) - 1, Data(paket));

			//Send Segment TODO: SLIDING WINDOW
			char* segment = (char *) &paket;
			if (sendto(udpSocket, segment, sizeof(paket), 0, (struct sockaddr *) &clientAddress, slen) == -1) {
				die("sendto");
			}
			paramSend++;
		}
		printf("YAHA\n");
			
		//Sending Lost Frame
		int i;
		for (i = repeatStart; i < repeatEnd; i++) {
			paket = CreateSegment(i, buffer[i - bufferSizeOffset], 0);
			Checksum(paket) = generateChecksumPaket(paket);
			
			printf("Repeat Send Frame Number %d : DATA %c\n", i, Data(paket));

			char* segment = (char *) &paket;
			if (sendto(udpSocket, segment, sizeof(paket), 0, (struct sockaddr *) &clientAddress, slen) == -1) {
				die("sendto");
			}
			paramSend++;
		}

		//Receive ACK
		for (i = 0; i < paramSend; i++) {
			char* acksegment = (char *) &ack;
			if (recvfrom(udpSocket, acksegment, sizeof(ack), 0, (struct sockaddr*) &clientAddress, &slen) >= 0) {
				if (Checksum(ack) == generateChecksumACK(ack)) {
					advertisedWindowSize = AdvertisedWindowSize(ack);
					LAR(sendingWindow) = NextSequenceNumber(ack);
				} else {
					printf("Wrong ACK Checksum\n");
				}
			} else {
				printf("Timeout\n");
			}
			printf("Received ACK %d\n", NextSequenceNumber(ack));
		}
		

		if (alreadyReadAll == 1 && LAR(sendingWindow) > counterBuffer + bufferSizeOffset - 1) {
			break;
		}

		//Buffer Size Offset Increase
		printf("YEEE %d %d\n", LAR(sendingWindow),  bufferSize + bufferSizeOffset);
		if (LAR(sendingWindow) == bufferSize + bufferSizeOffset) {
			counterBuffer = 0;
			bufferSizeOffset += bufferSize;
			printf("SIKAT %d\n", bufferSizeOffset);
		} 

		// sleep(rand() % 2);
	}

	//Sending Ending Sequence
	PacketACK finalACK;
	NextSequenceNumber(finalACK) = 0;
	Segment finalSegment;
	finalSegment = CreateSegment(0, 0, 0);
	SOH(finalSegment) = 0x2;
	Checksum(finalSegment) = generateChecksumPaket(finalSegment);
	printf("%c %c\n", generateChecksumPaket(finalSegment), Checksum(finalSegment));
	cout <<(generateChecksumPaket(finalSegment) == Checksum(finalSegment)) << endl;
	cout << NextSequenceNumber(finalACK) << endl;
	while (NextSequenceNumber(finalACK) == 0 || generateChecksumACK(finalACK) != Checksum(finalACK)) {
		printf("Sending Final\n");
		char* segment = (char *) &finalSegment;
		sendto(udpSocket, segment, sizeof(finalSegment), 0, (struct sockaddr *) &clientAddress, slen);

		char* acksegment = (char *) &finalACK;
		recvfrom(udpSocket, acksegment, sizeof(finalACK), 0, (struct sockaddr*) &clientAddress, &slen);
	}

	printf("Sending Data Already Successful\n");

	close(udpSocket);
}