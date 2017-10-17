#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utility.h"
#include <fstream>
#include <math.h>
#include <unistd.h>

using namespace std;

unsigned int bufferSize;

void die(char *s) {
	perror(s);
	exit(1);
}

void flush(bool *alreadyReceived) {
	int i;
	for (i = 0; i < bufferSize; i++) {
		alreadyReceived[i] = false;
	}
}

int main(int argc, char* argv[]) {
	char* fileName = argv[1];
	unsigned int windowSize = atoi(argv[2]);
	bufferSize = atoi(argv[3]);
	int port = atoi(argv[4]);

	printf("%s\n%d\n%d\n%d\n", fileName, windowSize, bufferSize, port);
	unsigned char buffer[bufferSize];
	bool alreadyReceived[bufferSize];

	//Urusan Socket
	int udpSocket;
	struct sockaddr_in serverAddr, clientAddr;
	unsigned int slen = sizeof(serverAddr); 

	if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		die("socket");
	}

	//Urusan Timeout
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10;
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

	flush(alreadyReceived);
	
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
		
				printf("SeqNum %d LFR %d LAF %d\n", SequenceNumber(paket), LFR, LAF);
				if (SequenceNumber(paket) >= LFR && SequenceNumber(paket) <= LAF) {
					if (SequenceNumber(paket) == LFR) LFR++;

					printf("SeqNum %d LFR %d LAF %d counterBufferOffset %d\n", SequenceNumber(paket), LFR, LAF, counterBufferOffset);
					
					//Writing Data to Buffer
					if (SequenceNumber(paket) - counterBufferOffset >= 0) {
						if (!alreadyReceived[SequenceNumber(paket) - counterBufferOffset]) {
							alreadyReceived[SequenceNumber(paket) - counterBufferOffset] = true;
							buffer[SequenceNumber(paket) - counterBufferOffset] = Data(paket);
							counterBuffer++;
							advertisedWindowSize--;
							if (advertisedWindowSize == 0) {
								advertisedWindowSize = (bufferSize > windowSize) ? windowSize : bufferSize;
							}
						}	
					}

				}

				printf("windowSize=%d\n", windowSize);
				printf("advertisedWindowSize=%d\n", advertisedWindowSize);
				LAF = LFR + min(windowSize, advertisedWindowSize);
				printf("LAF=%d\n", LAF);
				printf("LFR=%d\n", LFR);
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
		printf("Counter Buffer %d bufferSize %d\n", counterBuffer, bufferSize);
		if (counterBuffer == bufferSize) {
			printf("MASUK SINI GING\n");
			counterBufferOffset += bufferSize;
			counterBuffer = 0;
			for (int i = 0; i < bufferSize; i++) {
				fputc(buffer[i], file);	
			}
			flush(alreadyReceived);
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