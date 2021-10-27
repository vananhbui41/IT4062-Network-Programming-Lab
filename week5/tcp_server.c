#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

// #define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 2   /* Number of allowed connections */
//#define BUFF_SIZE 1024
#define MAX 100


typedef struct node{
	char username[MAX];
	char pass[MAX];
	int status;
	struct node *next;
} node_a;

// file -> linked list 
node_a *loadData(char *filename){
	int status, count =0;
	FILE *f;
	char username[MAX], pass[MAX];
	node_a *head, *current;
	head = current = NULL;

	printf("Loading data...\n");
	// open file
	if((f = fopen(filename,"r"))==NULL){
		printf("Failed to open file");
		exit(0);
	}

	//data -> linked list
	while(fscanf(f,"%s %s %d\n", username,pass,&status) != EOF){
		node_a *node = malloc(sizeof(node_a));
		strcpy(node->username, username);
		strcpy(node->pass,pass);
		node->status = status;

		if(head == NULL)
			current = head = node;
		else
			current = current->next = node;
		count++;
	}

	fclose(f);
	printf("Successfully loaded %d account(s)\n",count);
	return head;
}

//find account
node_a *findNode(node_a *head, char *username){
	node_a *current = head;
	while(current != NULL){
		if (0 == strcmp(current->username, username))
			return current;
		current = current->next;
	}

	return NULL;
}

// linked list -> file
void saveData(node_a *head, char *filename){
	FILE *f;
	f = fopen(filename,"w");
	node_a *current;
	for (current = head; current; current = current->next)
		fprintf(f, "%s %s %d\n", current->username, current->pass, current->status);
	fclose(f);
}


int main(int argc, char const *argv[])
{
 	char filename[] = "account.txt";

 	// check argument
 	if(argc != 2){
 		printf("Invalid argument\n\n");
 		return 0;
 	}


	int listen_sock, conn_sock; /* file descriptors */
	char username[MAX], pass[MAX], *reply;
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	int pid;
	node_a *found;
	
	node_a *account_list = loadData(filename);

	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		pid = fork();
		if (pid < 0){
			perror("Error");
			return 1;
		}

		if (pid == 0){
						// receive username
			if (0 >= (bytes_received = recv(conn_sock, username, MAX-1, 0))) {
				printf("\nConnection closed\n");
				break;
			}
			username[bytes_received] = '\0';

			// check username existence
			if ((found = findNode(account_list, username))) {
				if (found->status == 1)
					reply = "1"; // username found
				else
					reply = "2"; // username found but has been locked
			}
			else
				reply = "0"; // username not found

			// echo to client
			if (0 >= (bytes_sent = send(conn_sock, reply, strlen(reply), 0))) {
				printf("\nConnection closed\n");
				break;
			}

			int count = 0; // count pass repeatation

			while (1) {
				// receive pass
				memset(pass, '\0', MAX);
				if (0 >= (bytes_received = recv(conn_sock, pass, MAX-1, 0))) {
					printf("\nConnection closed\n");
					break;
				}
				pass[bytes_received] = '\0';

				// validate pass
				if (0 == strcmp(found->pass, pass))
					reply = "1"; // pass valid, reply 1
				else {
					count++;
					if (count == 3) {
						reply = "2"; // wrong pass 3 times, reply 2
						found->status = 0; // then lock account
					}
					else
						reply = "0"; // wrong pass < 3 times, reply 0
				}

				// echo to client
				if (0 >= (bytes_sent = send(conn_sock, reply, strlen(reply), 0))) {
					printf("\nConnection closed . Bye \n");
					break;
				}
			}
			// save linked list state
			saveData(account_list, filename);
			close(conn_sock);
		} else{
			close(conn_sock);
		}
	}	
	close(listen_sock);
	return 0;
}
