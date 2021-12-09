all: client server 

client: client-socket.c
	gcc client-socket.c -o client

server: server-socket.c
	gcc -pthread server-socket.c -o server

