#ifndef _UTILITY_H_
#define _UTILITY_H_


/* *** MODULE *** */
#include <string.h>
#include <stdint.h>

/* *** DATA STRUCTURE *** */
typedef struct {
  unsigned char SOH;
  int SequenceNumber;
  unsigned char STX;
  unsigned char Data;
  unsigned char ETX;
  unsigned char Checksum;
} Segment;

typedef struct {
  unsigned char ACK;
  int NextSequenceNumber;
  unsigned char AdvertisedWindowSize;
  unsigned char Checksum;
} PacketACK;

/* Definition */
#define DefaultSOH 0x1;
#define DefaultSTX 0x2;
#define DefaultETX 0x3;
#define DefaultACK 0x6;

/* Selector */
#define SOH(S) ((S).SOH)
#define SequenceNumber(S) ((S).SequenceNumber)
#define STX(S) ((S).STX)
#define Data(S) ((S).Data)
#define ETX(S) ((S).ETX)
#define Checksum(S) ((S).Checksum)
#define ACK(P) ((P).ACK)
#define NextSequenceNumber(P) ((P).NextSequenceNumber)
#define AdvertisedWindowSize(P) ((P).AdvertisedWindowSize)


/* *** PROTOTYPE *** */
unsigned char* ReadData(char* fileName);

Segment CreateSegment(int inputSequenceNumber, unsigned char inputData, unsigned char inputChecksum);

PacketACK CreatePacketACK(int inputNextSequenceNumber, unsigned char inputAdvertisedWindowSize, unsigned char inputChecksum);

#endif