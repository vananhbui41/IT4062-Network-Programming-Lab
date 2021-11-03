#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
// #include "transfer.h"


#define BUFFSIZE 4096
#define MAX_LINE 4096
#define NOT_EXIST "not exist"
#define EXIST "exist"

void writefile(int sockfd, FILE *fp);
char *makePath(char *fileName);
ssize_t total=0;
int main(int argc, char *argv[]) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int serverPort = atoi(argv[1]);
    if (sockfd == -1) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }
    
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(serverPort);

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) 
    {
        perror("Bind Error");
        exit(1);
    }

    if (listen(sockfd, serverPort) == -1) 
    {
        perror("Listen Error");
        exit(1);
    }

    socklen_t addrlen = sizeof(clientaddr);
    int connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
    if (connfd == -1) 
    {
        perror("Connect Error");
        exit(1);
    }
    close(sockfd); 

    char filename[BUFFSIZE] = {0}; 

    if (recv(connfd, filename, BUFFSIZE, 0) == -1) 
    {
        perror("Can't receive filename");
        exit(1);
    }


    char path[100];

    strcat(path, filename);  

    FILE *fp1 = fopen(path, "r");
    if(fp1!=NULL){
        printf("File da ton tai!!\n");
        fclose(fp1);
        send(connfd, EXIST, strlen(EXIST), 0);
        exit(1);
        } ;
    // fclose(fp1);
    send(connfd, NOT_EXIST, strlen(NOT_EXIST), 0);
    FILE *fp = fopen(path, "wb");
    if (fp == NULL) 
    {
        perror("Can't open file");
        exit(1);
    }
    
    char addr[INET_ADDRSTRLEN];
    printf("Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
    writefile(connfd, fp);
    printf("Receive Success, NumBytes = %ld\n", total);
    
    // fclose(fp1);
    fclose(fp);
    close(connfd);
    return 0;
}



void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) 
    {
        total+=n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }
        
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
}

