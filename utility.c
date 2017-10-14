#include <stdio.h>
#include "utility.h"
#include <stdlib.h>

unsigned char* ReadData(char* fileName) {
  FILE* file;
  file = fopen(fileName, "r");
  if(file == NULL) {
    return NULL;
  }
  int size = 0;
  unsigned char* data = (unsigned char*) malloc (sizeof(unsigned char));
  int c;
  while ((c = fgetc(file)) != EOF) {
    data[size] = (unsigned char) c;
    size++;
  }
  fclose(file);

  return data;
}

Segment CreateSegment(int inputSequenceNumber, unsigned char inputData, unsigned char inputChecksum) {
  Segment S;
  SOH(S) = DefaultSOH;
  SequenceNumber(S) = inputSequenceNumber;
  STX(S) = DefaultSTX;
  Data(S) = inputData;
  ETX(S) = DefaultETX;
  Checksum(S) = inputChecksum;

  return S;
}

PacketACK CreatePacketACK(int inputNextSequenceNumber, unsigned char inputAdvertisedWindowSize, unsigned char inputChecksum) {
  PacketACK P;
  ACK(P) = DefaultACK;
  NextSequenceNumber(P) = inputNextSequenceNumber;
  AdvertisedWindowSize(P) = inputAdvertisedWindowSize;
  Checksum(P) = inputChecksum;

  return P;
}

int main() {
  unsigned char* data = ReadData("hehe.txt");
  if(data != NULL) {
    printf("Size of data: %d\n", sizeof(data));
    for(int i = 0; i < sizeof(data); i++) {
      printf("%d. %c\n", i, data[i]);
    }
  } else {
    printf("return null");
  }

  return 0;
}