all: client server

client: tcp-client.c
	gcc tcp-client.c -o client

server: tcp-server.c
	gcc tcp-server.c -o server

clean:
	rm -f *.o