#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFSIZE 4096
#define MAX_LINE 4096
#define NOT_EXIST "not exist"
#define EXIST "exist"

ssize_t total=0;

void sendfile(FILE *fp, int client_sock) 
{
    int n; 
    char sendline[MAX_LINE] = {0}; 
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) 
    {
        total+=n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        if (send(client_sock, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}


int checkFormat(char buf[])
{
    char ext[10], *p, s[50];
    strcpy(s, buf);
    p = strtok (s, ".");

    while (p != NULL)
    {
        strcpy(ext, p);
        p = strtok (NULL, "."); 
    }

    if(strcmp(ext,"jpg")==0||strcmp(ext,"png")==0||strcmp(ext,"bmp")==0) return 1;
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 4) 
    {
        perror("input inivalid");
        exit(1);
    }

    char filename[50];
    strcpy(filename, argv[3]);

    if(checkFormat(filename)==0) {
        printf("Error: Wrong file format\n");
        exit(1);
    }

    char *fileName = basename(argv[3]); 
    puts(filename);
    if (fileName == NULL)
    {
        perror("Error: File not found");
        exit(1);
    }

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    int serverPort = atoi(argv[2]);
    if (client_sock < 0) 
    {
        perror("Can't allocate client_sock");
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 0)
    {
        perror("IPaddress Convert Error");
        exit(1);
    }

    if (connect(client_sock, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    
    
    char buff[BUFFSIZE] = {0};
    strncpy(buff, fileName, strlen(fileName));
    if (send(client_sock, buff, BUFFSIZE, 0) == -1)
    {
        perror("Can't send fileName");
        exit(1);
    }
    char result[10];

    recv(client_sock, result, sizeof(result), 0);
    if(strcmp(result,EXIST)==0) {
        printf("Error: File already exists!!\n");
        close(client_sock);
        return 0;
    }
    
    FILE *fp = fopen(argv[3], "rb");
    if (fp == NULL) 
    {
        printf("Error: File not found!\n");
        exit(1);
    }

    sendfile(fp, client_sock);
    printf("Success: File transfer is completed\n");
    fclose(fp);
    close(client_sock);
    return 0;
}



