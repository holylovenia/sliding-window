CC=g++

default: sendfile recvfile

sendfile: sender.o utility.o
	$(CC) -o sendfile sender.o utility.o

recvfile: receiver.o utility.o
	$(CC) -o recvfile receiver.o utility.o

clean: 
	rm -f *.o