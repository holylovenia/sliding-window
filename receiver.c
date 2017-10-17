#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utility.h"

void die(char *s) {
	perror(s);
	exit(1);
}

int main(int argc, char* argv[]) {
	char* fileName = argv[1];
	int windowSize = atoi(argv[2]);
	int bufferSize = atoi(argv[3]);
	int port = atoi(argv[4]);

	printf("%s\n%d\n%d\n%d\n", fileName, windowSize, bufferSize, port);
	unsigned char buffer[bufferSize];
	
	int udpSocket;
	struct sockaddr_in serverAddr, clientAddr;
	int slen = sizeof(serverAddr); 

	if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		die("socket");
	}
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(port);
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind (udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) == -1) {
		die("bind");
	}


	int recv_len;
	Segment paket;
	while (1) {
		printf("Waiting for data...\n");
		fflush(stdout);

		char * recvBuf = (char *) &paket;
		if (recv_len = recvfrom(udpSocket, recvBuf, sizeof(paket), 0, (struct sockaddr *) &serverAddr, &slen) == -1) {
			die("recvfrom");
		}

		printf("Received packet from %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
		printf("Data: %c\n", Data(paket));
		
		if (Data(paket) + Checksum(paket) != 0xFF) {
			ack[0] = 0;
		} else {
			ack[0] = 1;
		}
		if (sendto(udpSocket, ack, 7, 0, (struct sockaddr*) &serverAddr, slen) == -1) {
			die("sendto");
		}
	}
	printf("All Data Received Succesfully\n");

	//WritetoFile

}