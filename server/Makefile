CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread

all: server.o DateTime.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(ECFLAGS) -I ../socket
clean:
	rm -rf *.o