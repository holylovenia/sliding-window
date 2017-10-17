CC=g++

default: sendfile recvfile

sender.o: sender.cpp utility.h send_window.h
	$(CC) -c sender.cpp

utility.o: utility.cpp utility.h
	$(CC) -c utility.cpp

receiver.o: receiver.cpp utility.h
	$(CC) -c receiver.cpp

sendfile: sender.o utility.o
	$(CC) -o sendfile sender.o utility.o

recvfile: receiver.o utility.o
	$(CC) -o recvfile receiver.o utility.o

clean: 
	rm -f *.o