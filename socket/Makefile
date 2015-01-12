CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread

all: socket_link2.o socket_utils2.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(ECFLAGS)
clean:
	rm *.o