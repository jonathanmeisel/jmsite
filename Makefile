CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread
SERVER=server.o 
SOCKET=socket_link2.o socket_utils2.o 
WEBSERVER=webserver.o ProcessManager.o server_utils.o
HTTP=http.o

all: webserver 

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(ECFLAGS)

webserver: $(WEBSERVER) $(HTTP) $(SERVER) $(SOCKET)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf webserver *.o
