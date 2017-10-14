#include <fstream>
#include <iostream>
#include "utility.h"
#include <cstring>

unsigned char* readData(string fileName) {
  ifstream file(fileName.c_str());
  size_t size = 0;

  file.seekg(0, ios::end);
  size = file.tellg();
  file.seekg(0, ios::beg);

  char* temp = new char[size];
  
  if(file) {
    file.read(temp, size);
    unsigned char* data = new unsigned char[size];
    strcpy((char*) data, temp);
    return data;
  } else {
    return NULL;
  }
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