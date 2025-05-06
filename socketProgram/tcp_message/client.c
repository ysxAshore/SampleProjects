#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include "./include/wrap.h"

#define MAXLINE 80
#define SERV_PORT 8000

int main(int argc,char *argv[]) {
    struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;
	char str[MAXLINE];
    
    
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
    
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    fputs("please input something: ",stdout);
    while(fgets(str,MAXLINE,stdin) != NULL){

        Write(sockfd, str, strlen(str));
	    n = Read(sockfd, buf, MAXLINE);
	    printf("Response from server:\n");
	    write(STDOUT_FILENO, buf, n);
        fputs("please input something: ",stdout);
        
    }

	Close(sockfd);
	return 0;
}

