#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024

typedef struct 
{
	int opcode;
	int length;
	char payload[1024];
} message;

void send_file(FILE *fp, int sockfd, message *mess)
{
	int n;
	int total_byte = 0;
	char data[BUFF_SIZE] = "";
	mess->opcode = 2;

	while ((fgets(data, BUFF_SIZE, fp)) != NULL)
	{
		mess->length = strlen(data);
		strcpy(mess->payload, data);
		if (send(sockfd, mess, sizeof(message), 0) < 0)
		{
			perror("Failed to send file");
			exit(1);
		}
	}

	mess->length = 0;
	strcpy(mess->payload, "\0");
	if (send(sockfd, mess, sizeof(message), 0) < 0)
	{
		perror("Failed to send file");
		exit(1);
	}
	// printf("Send file complete\n");
}

int main(int argc, char *argv[]){
	// check param
	if (argc != 3){
		printf("Syntax Error !!! Syntax: ./client IPAddress Port_number\n");
		exit(1);
	}

	int request;
	int key;
	char filePath[BUFF_SIZE];


	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	
	//send message
	do
	{
		printf("Ma hoa(0)/ Giai ma(1):");
		scanf("%d",&request);
	} while (request<0 || request >1);
	
	printf("Nhap gia tri khoa: ");
	scanf("%d",&key);

	printf("Nhap ten file: ");
	getchar();
	memset(filePath, '\0',(strlen(filePath)+1));
	fgets(filePath, BUFF_SIZE, stdin);		
	filePath[strlen(filePath)-1] = '\0';

	message *key_mess = (message *)malloc(sizeof(message));
	sprintf(key_mess->payload, "%d", key);
	key_mess->opcode = request;
	bytes_sent = send(client_sock, key_mess, sizeof(message), 0);
	if (bytes_sent < 0)
		perror("\nError: ");
	free(key_mess);

	FILE *fptr = fopen(filePath, "r");
	message *file_mess = (message*)malloc(sizeof(message));
	send_file(fptr, client_sock, file_mess);
	fclose(fptr);
	free(file_mess);

	printf("Ket qua tu server:\n");
	message *data_mess = (message*)malloc(sizeof(message));

	do
	{
		bytes_received = recv(client_sock, data_mess, sizeof(message), 0);
		if (bytes_received <= 0)
			perror("\nError: ");
		else if (bytes_received == 0)
			perror("Connection closed.\n");
		else
		{
			if (data_mess->length == 0 && data_mess->opcode == 2)
			{
				break;
			}
			data_mess->payload[bytes_received] = '\0';
			printf("%s", data_mess->payload);
		}
	} while (bytes_received > 0);
	printf("\n");
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
