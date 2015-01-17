CC=g++
CFLAGS=-std=c++11 -g
ECFLAGS=-pthread
WEBDIR=web
SERVERDIR=server
SOCKETDIR=socket
SERVER=server/server.o server/DateTime.o
SOCKET=socket/socket_link2.o socket/socket_utils2.o 
WEBSERVER=web/webserver.o web/ProcessManager.o web/server_utils.o web/http.cpp

all:
	make -C $(SOCKETDIR)
	make -C $(WEBDIR)
	make -C $(SERVERDIR)
	$(CC) -o webserver $(SERVER) $(SOCKET) $(WEBSERVER) $(CFLAGS)

clean:
	rm -rf webserver
	make -C $(WEBDIR) clean
	make -C $(SERVERDIR) clean
	make -C $(SOCKETDIR) clean
