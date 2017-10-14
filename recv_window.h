#ifndef RECV_WINDOW_H
#define RECV_WINDOW_H

typedef struct {
  int maxReceivingWindowSize;
  int receivingWindowSize;
  int lastFrameReceived;
  int largestAcceptableFrame;
} ReceivingWindow;

#define NONE 0

#define maxRWS(R) (R).maxReceivingWindowSize
#define RWS(R) (R).receivingWindowSize
#define LFR(R) (R).lastFrameReceived
#define LAF(R) (R).largestAcceptableFrame

void createNew(ReceivingWindow r, int maxWindowSize) {
  maxRWS(r) = maxWindowSize;
  RWS(r) = maxWindowSize;
  LFR(r) = NONE;
  LAF(r) = NONE;
}

void createNew(ReceivingWindow r, int maxWindowSize, int lastFrameReceived, int largestAcceptableFrame) {
  maxRWS(r) = maxWindowSize;
  RWS(r) = maxWindowSize;
  LFR(r) = lastFrameReceived;
  LAF(r) = largestAcceptableFrame;
}

void setSize(ReceivingWindow r, int size) {
  size = (size < maxRWS(r)) ? size : maxRWS(r);
  RWS(r) = size;
}

void receiveFrames(ReceivingWindow r, int frame) {
  LFR(r) = (LFR(r) < frame) ? frame : LFR(r);
}

void sendAcks(ReceivingWindow r, int ack) {
  // Validate ack < RWS(r)
  ack = (ack < RWS(r)) ? ack : RWS(r);

  // Validate LAF - LFR <= RWS
  if (LAF(r) + ack > RWS(r) + LFR(r)) {
    // Sending too many acks, reduce count
    ack = RWS(r) + LFR(r) - LAF(r);
  }

  LAF(r) += ack;
}

#endif