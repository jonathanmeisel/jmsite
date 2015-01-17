CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread

all: webserver.o ProcessManager.o server_utils.o http.o 

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(ECFLAGS) -I ../server -I ../socket

clean:
	rm -rf *.o