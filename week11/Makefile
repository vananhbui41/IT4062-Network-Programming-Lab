all: client server

client: tcp_client.c
	gcc tcp_client.c -o client

server: select_server.c
	gcc select_server.c -o server

clean: 
	rm *.o -f