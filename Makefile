CC=g++

default: sendfile recvfile

all: sendfile recvfile

sender.o: src/sender.cpp src/utility.h src/send_window.h
	$(CC) -c src/sender.cpp

utility.o: src/utility.cpp src/utility.h
	$(CC) -c src/utility.cpp

receiver.o: src/receiver.cpp src/utility.h
	$(CC) -c src/receiver.cpp

sendfile: sender.o utility.o
	$(CC) -o sendfile sender.o utility.o

recvfile: receiver.o utility.o
	$(CC) -o recvfile receiver.o utility.o

clean: 
	rm -f *.o
