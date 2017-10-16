#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utility.h"
#include <fstream>
#include <math.h>
#include <unistd.h>


unsigned int bufferSize;

using namespace std;

void die(char *s) {
	perror(s);
	exit(1);
}

void flush(unsigned char *buffer) {
	int i;
	for (i = 0; i < bufferSize; i++) {
		buffer[i] = '\0';
	}
}

int main(int argc, char* argv[]) {
	char* fileName = argv[1];
	unsigned int windowSize = atoi(argv[2]);
	bufferSize = atoi(argv[3]);
	int port = atoi(argv[4]);

	printf("%s\n%d\n%d\n%d\n", fileName, windowSize, bufferSize, port);
	unsigned char buffer[bufferSize];
	
	//Urusan Socket
	int udpSocket;
	struct sockaddr_in serverAddr, clientAddr;
	unsigned int slen = sizeof(serverAddr); 

	if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		die("socket");
	}
	//Urusan Timeout

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

	//Urusan Address
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(port);
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) == -1) {
		die("bind");
	}

	//Urusan File
	FILE* file = fopen(fileName, "w");

	//Urusan Receive
	int recv_len;
	Segment paket;
	PacketACK ack;
	unsigned int lastFrameCorrect = 0;
	unsigned int LFR = 0;
	bool first = true;
	unsigned int LAF = windowSize;
	unsigned int counterBuffer = 0;
	unsigned int counterBufferOffset = 0;
	unsigned int advertisedWindowSize = (bufferSize > windowSize) ? windowSize : bufferSize;
	int finish = 0;

	flush(buffer);
	
	//Urusan Random Number
	srand(time(NULL));
	while (1) {
		printf("Waiting for data...\n");
		fflush(stdout);

		char * recvBuf = (char *) &paket;
		if (recvfrom(udpSocket, recvBuf, sizeof(paket), 0, (struct sockaddr *) &serverAddr, &slen) >= 0) {
			printf("%c %c\n", generateChecksumPaket(paket), Checksum(paket));
			if (generateChecksumPaket(paket) == Checksum(paket)) {
				if (SOH(paket) == 0x02) {
					finish = 1;
				}
				printf("Received packet from %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
				printf("Frame Number: %d Data: %c\n", SequenceNumber(paket), Data(paket));
		
				if (SequenceNumber(paket) >= LFR && SequenceNumber(paket) <= LAF) {
					if (SequenceNumber(paket) == LFR) LFR++;
						
					//Writing Data to Buffer
					if (buffer[SequenceNumber(paket) - counterBufferOffset] == '\0') {
						buffer[SequenceNumber(paket) - counterBufferOffset] = Data(paket);
						counterBuffer++;
						advertisedWindowSize--;
						if (advertisedWindowSize == 0) {
							advertisedWindowSize = bufferSize;
						}
					}
				}
				LAF = LFR + min(windowSize, advertisedWindowSize);
			} else {
				printf("Wrong Checksum\n");
			} 
		} else {
			printf("Fail to receive\n");
		}

		//Sending ACK 
		printf("Send ACK %d\n", LFR);
		ack = CreatePacketACK(LFR, advertisedWindowSize, '0');
		Checksum(ack) = generateChecksumACK(ack);

		char *sendBuf = (char *) &ack;
		sendto(udpSocket, sendBuf, sizeof(ack), 0, (struct sockaddr*) &serverAddr, slen);

		//Writing Data to File
		if (counterBuffer == bufferSize) {
			counterBufferOffset += bufferSize;
			counterBuffer = 0;
			for (int i = 0; i < bufferSize; i++) {
				fputc(buffer[i], file);	
			}
			flush(buffer); 
			printf("Writing File\n");
		}

		// sleep(rand() % 3);

		if (finish) {
			break;
		}
	}
	if (counterBuffer != 0) {
		printf("Print Remaining Buffer to File");
		for (int i = 0; i < counterBuffer; i++) {
			fputc(buffer[i], file);
		}
	}

	printf("All Data Received Succesfully\n");
	
	fclose(file);	
}