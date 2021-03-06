#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

//Remember to use -pthread when compiling this server's source code
void *connection_handler(void *);

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


int main()
{
    // char server_message[100] = "Hello from Server!!\n";
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
    if (server_socket==-1){
	perror("Socket initialisation failed");
	exit(EXIT_FAILURE);
	}
    else
	printf("Server socket created successfully\n");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = INADDR_ANY; //server ch???p nh???n b???t k?? ?????a ch??? IP n??o ?????n 
    
    //bind the socket to the specified IP addr and port
	// chi???m c???ng tr??n server
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))!=0){
	printf("socket bind failed...\n"); 
        exit(0);
	}
    else
	printf("Socket successfully binded..\n"); 
    // l???ng nghe tr??n d???ch v???
    if (listen(server_socket, 3)!=0){ //c??? ?????nh s??? lu???ng l?? 3
		printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    int no_threads=0;
    pthread_t threads[3];
    while (no_threads<3){ //n???u s??? l?????ng thread t???o ra <3 => ti???p t???c x??? l??
	printf("Listening...\n");
	int client_socket = accept(server_socket, NULL, NULL);// l???ng nghe k???t n???i client
	puts("Connection accepted");
	if( pthread_create( &threads[no_threads], NULL ,  connection_handler , &client_socket) < 0){//t???o thread x??? l?? k???t n???i client
	//qu?? tr??nh x??? l?? n???m trong connection_handler
	perror("Could not create thread");//Kh??ng t???o ra ???????c lu???ng
	return 1;}
    	if (client_socket < 0) { 
        	printf("server acccept failed...\n"); 
        	exit(0); 
    		} 
    	else
        	printf("Server acccept the client...\n");
	puts("Handler assigned");
	no_threads++;// t???o lu???ng th??nh c??ng => t??ng gi?? tr??? s??? lu???ng
	}
	int k=0;
    for (k=0;k<3;k++){
		pthread_join(threads[k],NULL);//lu???ng m???i l???y l???i t??i nguy??n c???a lu???ng c?? v?? lu???ng c?? v???n ch??a gi???i ph??ng t??i nguy??n
	}

    //int send_status;
    //send_status=send(client_socket, server_message, sizeof(server_message), 0);
    close(server_socket);
    
    return 0;
}

// H??m x??? l?? d??? li???u t??? client
// Nh???n ??o???n text t??? client v?? g???i l???i th??ng ??i???p cho client
void *connection_handler(void *client_socket){
	int socket = *(int*) client_socket;
	int read_len;
	char server_message[100]="Hello from server\n";
	int send_status;
    	send_status=send(socket, server_message, sizeof(server_message), 0);
	char client_message[100];
	while( (read_len=recv(socket,client_message, 100,0))>0)
	{
		//end of string marker
		client_message[read_len] = '\0';
		if((strcmp(client_message,"q")==0) || (strcmp(client_message,"Q")==0) ){break;}
		//Send the message back to client
		for (int i=0; i < strlen(client_message); i++){
			if (!isalpha(client_message[i])){
				strcpy(client_message,"Wrong format text\n");
				break;
			}
			else {
				client_message[i] = toupper(client_message[i]);
			}
		}
		send_status=send(socket , client_message , strlen(client_message),0);	
	}
	
	return 0;
}

