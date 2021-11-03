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

void sendfile(FILE *fp, int sockfd);

int checkImg(char buf[]);
ssize_t total=0;
int main(int argc, char* argv[])
{
    if (argc != 4) 
    {
        perror("input inivalid");
        exit(1);
    }

    char fileName[50];
    strcpy(fileName, argv[3]);

    if(checkImg(fileName)==0) {
        printf("Error: Wrong file format\n");
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int serverPort = atoi(argv[2]);
    if (sockfd < 0) 
    {
        perror("Can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) < 0)
    {
        perror("IPaddress Convert Error");
        exit(1);
    }

    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Connect Error");
        exit(1);
    }
    
    char *filename = basename(argv[3]); 
    puts(fileName);
    if (filename == NULL)
    {
        perror("Error: File not found");
        exit(1);
    }
    
    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    if (send(sockfd, buff, BUFFSIZE, 0) == -1)
    {
        perror("Can't send filename");
        exit(1);
    }
    char result[10];

    recv(sockfd, result, sizeof(result), 0);
    if(strcmp(result,EXIST)==0) {
        printf("Error: File already exists!!\n");
        close(sockfd);
        return 0;
    }
    
    FILE *fp = fopen(argv[3], "rb");
    if (fp == NULL) 
    {
        printf("Error: File not found!!");
        exit(1);
    }

    sendfile(fp, sockfd);
    // printf("Send Success, NumBytes = %ld\n", total);
    printf("Success: File transfer is completed\n");
    fclose(fp);
    close(sockfd);
    return 0;
}

void sendfile(FILE *fp, int sockfd) 
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
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}

int checkImg(char buf[])
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
