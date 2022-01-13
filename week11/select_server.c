#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5500   /* Port that will be opened */ 
#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define PATH "temp.txt"

typedef struct {
    int opcode;
    int length;
    char payload[1024];
} message;

int Mahoa_Ceaser(char Str_Nguon[],char Str_Mahoa[],int len,int key)
{
 for (int i=0;i<len;i++)
 {
  char c=Str_Nguon[i];
  if(c>='A' && c<='Z')
   Str_Mahoa[i]=(Str_Nguon[i]-'A'+key)%26 +'A';
  else if(c>='a' && c<='z')
   Str_Mahoa[i]=(Str_Nguon[i]-'a'+key)%26+'a';
  else Str_Mahoa[i]=Str_Nguon[i];
 }
 return 0;
}

int Giaima_Ceaser(char Str_Nguon[],char Str_Giaima[],int len,int key)
{
 for (int i=0;i<len;i++)
 {
  char c=Str_Nguon[i];
  if(c>='A' && c<='Z')
   Str_Giaima[i]=(abs(Str_Nguon[i]-'A'-key))%26 +'A';
  else if(c>='a' && c<='z')
   Str_Giaima[i]=(abs(Str_Nguon[i]-'a'-key))%26+'a';
  else Str_Giaima[i]=Str_Nguon[i];
 }
 return 0;
}

// int khoitao(char Str[])
// {
//  for(int i=0;i<SIZE;i++)
//   Str[i]=0;
//  return 0;
// }

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
			perror("[-]Error in sending file");
			exit(1);
		}
	}

	mess->length = 0;
	strcpy(mess->payload, "\0");
	if (send(sockfd, mess, sizeof(message), 0) < 0)
	{
		perror("[-]Error in sending file");
		exit(1);
	}
	// printf("Send file complete\n");
}



/* The processData function copies the input string to output */
void processData(char *in, char *out);

/* The recv() wrapper function*/
int receiveData(int s, message *mess, int flags);

/* The send() wrapper function*/
int sendData(int s, char *buff, int size, int flags);

int main(int argc, char *argv[])
{
	// check params
	if (argc != 2)
	{
		printf("Syntax Error !!! Syntax: ./server Port_number\n");
		exit(1);
	}
	

	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t	ret;
	fd_set	readfds, allset;
	char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {
				int key=0;
				int request = -1;

				message *recv_mess = (message *)malloc(sizeof(message));
				ret = receiveData(sockfd, recv_mess, 0);
				if (ret <= 0){
					FD_CLR(sockfd, &allset);
					close(sockfd);
					client[i] = -1;
				}
				else
				{
					request = recv_mess->opcode;
					key = atoi(recv_mess->payload);
				}
				
				FILE *fptr = fopen(PATH,"w");
				if (fptr == NULL)
				{
					printf("Cannot open file\n");
					exit(1);
				}

				do
				{
					char code[BUFF_SIZE] = "\0";
					ret = receiveData(sockfd, recv_mess, 0);
					if (ret <= 0)
					{
						FD_CLR(sockfd, &allset);
						close(sockfd);
						client[i] = -1;
					}
					else
					{
						if (recv_mess->length == 0 && recv_mess->opcode == 2)
						{
							break;
						}
						recv_mess->payload[ret] = '\0';
						if (request == 0)
							Mahoa_Ceaser(recv_mess->payload, code, strlen(recv_mess->payload), key);
						if (request == 1)
							Giaima_Ceaser(recv_mess->payload, code, strlen(recv_mess->payload), key);
						fprintf(fptr, "%s", code);
					}
				} while (ret>0);
				
				free(recv_mess);
				fclose(fptr);

				FILE *fp = fopen(PATH, "r");
				message *file_mess = (message *)malloc(sizeof(message));
				send_file(fp, sockfd, file_mess);
				fclose(fp);
				free(file_mess);
				
				if (remove(PATH) != 0)
				{
					printf("Error delete temp file\n");
				}

				FD_CLR(sockfd, &allset);
				close(sockfd);
				client[i] = -1;

				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
	
	return 0;
}

void processData(char *in, char *out){
	strcpy (out, in);
}

int receiveData(int s, message *mess, int flags)
{
	int n;
	n = recv(s, mess, sizeof(message), flags);
	if (n < 0)
		perror("Error");
	return n;
}

int sendData(int s, char *buff, int size, int flags){
	int n;
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

