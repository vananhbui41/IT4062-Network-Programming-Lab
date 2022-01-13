all: client server

client: tcp_client.c
	gcc tcp_client.c -o client

server: tcp_server_nbio.c
	gcc tcp_server_nbio.c -o server
