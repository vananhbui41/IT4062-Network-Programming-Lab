#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFSIZE 4096
#define MAX_LINE 4096
#define NOT_EXIST "not exist"
#define EXIST "exist"

ssize_t total = 0;

char *makePath(char *fileName);
void writeFile(int sockfd, FILE *fp){
	ssize_t n;
	char buff[MAX_LINE] = {0};
	while ((n = recv(sockfd, buff,MAX_LINE, 0))>0){
		total +=n;
		if (n == -1){
			perror("Error: Receive File Error!");
			exit(1);
		}

		if (fwrite(buff, sizeof(char), n, fp)!=n){
			perror("Error: Write File Error!");
			exit(1);
		}
		memset(buff,0,MAX_LINE);
	}
}

int main(int argc, char *argv[]){
	//Check param
	if (argc!=2){
		printf("Syntax: ./server PortNumber (Eq: ./server 5500)\n");
		exit(0);
	}

	int listen_sock, conn_sock;
	// int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: Failed to allocate sock");
		return 0;
	}

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: Bind Error");
		exit(0);
	}  

	if(listen(listen_sock, atoi(argv[1])) == -1){  /* calls listen() */
		perror("\nError: Listen Error");
		exit(0);
	}
	while(1){
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1){
			perror("\nError: ");
			break;
		} 
			
		char filename[BUFFSIZE] = {0}; 

	    if (recv(conn_sock, filename, BUFFSIZE, 0) == -1) {
	        perror("Can't receive filename!");
	        break;
	    }

		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */

		char path[MAX_LINE];

		strcat(path, filename);
		FILE *f1 = fopen(path,"r");
		if (f1!=NULL){
			printf("File already exists!\n");
			fclose(f1);
			send(conn_sock,EXIST,strlen(EXIST),0);
			exit(0);
		}

		send(conn_sock,NOT_EXIST, strlen(NOT_EXIST),0);
		FILE *f = fopen(path,"wb");
		if (f == NULL){
			perror("Failed to open file!\n");
			break;
		}

		char addr[INET_ADDRSTRLEN];
		printf("Start to receive file %s from %s\n",filename,inet_ntop(AF_INET, &client.sin_addr, addr, INET_ADDRSTRLEN));
		writeFile(conn_sock,f);
		printf("Success, NumBytes =%ld\n",total);

		fclose(f);
		close(conn_sock);
		return(0);
	}
	close(listen_sock);
	return 0;
	
}