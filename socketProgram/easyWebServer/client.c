#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../tcp_message/include/wrap.h"

#define MAXLINE 256
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
    if(fgets(str,MAXLINE,stdin) != NULL){ 
        proStringSpace(str);

        Write(sockfd, str, strlen(str));
	    printf("Response from server:\n");
        while((n = Read(sockfd,buf,MAXLINE)) > 0){
            buf[n] = '\0';
	        Write(STDOUT_FILENO,buf,n);
        }
     }

	Close(sockfd);
	return 0;
}
