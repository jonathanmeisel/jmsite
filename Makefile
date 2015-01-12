CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread
SERVER=server/server.o 
SOCKET=socket/socket_link2.o socket/socket_utils2.o 
WEBSERVER=web/webserver.o web/ProcessManager.o web/server_utils.o web/http.cpp

all: webserver 

webserver: $(WEBSERVER) $(SERVER) $(SOCKET)
	cd web; make;
	cd server; make;
	cd socket; make;

	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf webserver
	cd web; make clean;
	cd server; make clean;
	cd socket; make clean;
