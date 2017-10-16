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
  //Salah gan cma 1 byte ini
  unsigned char* data = (unsigned char*) malloc (sizeof(unsigned char));
  int c;
  while ((c = fgetc(file)) != EOF) {
    data[size] = (unsigned char) c;
    size++;
  }
  // Gak tau butuh apa gak
  data[size] = '\0';

  fclose(file);

  return data;
}

unsigned char generateChecksumACK(PacketACK paket) {
  unsigned char result = 0;
  result += ACK(paket);
  result += AdvertisedWindowSize(paket);
  result += NextSequenceNumber(paket) & 0xFF;
  return result;
}
 
unsigned char generateChecksumPaket(Segment paket) {
  unsigned char result = 0;
  result += SOH(paket);
  result += SequenceNumber(paket) & 0xFF;
  result += STX(paket);
  result += Data(paket);
  result += ETX(paket);
  return result;
}




Segment CreateSegment(unsigned int inputSequenceNumber, unsigned char inputData, unsigned char inputChecksum) {
  Segment S;
  SOH(S) = DefaultSOH;
  SequenceNumber(S) = inputSequenceNumber;
  STX(S) = DefaultSTX;
  Data(S) = inputData;
  ETX(S) = DefaultETX;
  Checksum(S) = inputChecksum;

  return S;
}

PacketACK CreatePacketACK(unsigned int inputNextSequenceNumber, unsigned char inputAdvertisedWindowSize, unsigned char inputChecksum) {
  PacketACK P;
  ACK(P) = DefaultACK;
  NextSequenceNumber(P) = inputNextSequenceNumber;
  AdvertisedWindowSize(P) = inputAdvertisedWindowSize;
  Checksum(P) = inputChecksum;

  return P;
}

// int main() {
//   unsigned char* data = ReadData("hehe.txt");
//   if(data != NULL) {
//     printf("Size of data: %d\n", sizeof(data));
//     for(int i = 0; i < sizeof(data); i++) {
//       printf("%d. %c\n", i, data[i]);
//     }
//   } else {
//     printf("return null");
//   }

//   return 0;
// }