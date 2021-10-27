#--*--Makefile--*--
all: client server

client: tcp_client.c
	gcc tcp_client.c -o client

server: tcp_server.c
	gcc tcp_server.c -o server

clean:
	rm -f *.o