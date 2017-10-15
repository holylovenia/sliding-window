#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "utility.h"

void die(char* errorMessage) {
	perror(errorMessage);
	exit(0);
}

int main(int argc, char* argv[]) {
	char* fileName = argv[1];
	int windowSize = atoi(argv[2]);
	int bufferSize = atoi(argv[3]);
	char* destIP = argv[4];
	int port = atoi(argv[5]);


	// Urusan Socket
	int udpSocket;
	struct sockaddr_in clientAddress;

	unsigned char buffer[bufferSize];
	
	FILE* file = fopen(fileName, "rb");
	if (file == NULL) {
		exit(0);
	}

	if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		die("socket");
	}

	memset((char *)&clientAddress, 0, sizeof(clientAddress));
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(port);
	clientAddress.sin_addr.s_addr = inet_addr(destIP);

	int slen = sizeof(clientAddress);
	int i = 0;


	//Urusan Segment dan Pengiriman
	Segment paket;
	PacketACK ack;
	int counterBuffer = 0;
	int counterSequence = 0;
	int alreadyReadAll = 0;
	unsigned char c;


	while (1) {
		printf("Sending Data: \n");

		//Urusan Buffer
		while (counterBuffer < bufferSize) {
			if (fscanf(file, "%c", &c) == EOF) {
				alreadyReadAll = 1;
				break;
			}
			buffer[counterBuffer] = c;
			counterBuffer++;
		}
		if (alreadyReadAll == 1) {
			break;
		}
		//Create Segment
		paket = CreateSegment(conterSequence, c, ~c);
		counterSequence++;
		printf("%c\n", Data(paket));

		//Send Segment TODO: SLIDING WINDOW
		char* segment = (char *) &paket;
		if (sendto(udpSocket, segment, sizeof(paket), 0, (struct sockaddr *) &clientAddress, slen) == -1) {
			die("sendto");
		} 

		//Receive ACK
		char* acksegment = (char *) &ack;
		if (recvfrom(udpSocket, acksegment, sizeof(ack), 0, (struct sockaddr*) &clientAddress, &slen) == -1) {
			die("recvfrom");
		}

		sleep(1);
	}
	printf("Sending Data Already Successful\n");
}