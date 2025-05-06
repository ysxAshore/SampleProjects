#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include "./include/wrap.h"

#define MAXLINE 80 //Buffer Size
#define SERV_PORT 8000 //service port

int main(int argc,char *argv[]) {

    // i indicate the client array loop index
    // maxi indicate the client array using items max index
    // maxfd indicate the current file describe symol max value
    // listenfd is the service socket function return value, use to accept function first parameter
    // connfd and is indicate the accept function return value, use to write and read function first parameter
    int i ,maxi, maxfd,listenfd,connfd,sockfd;

    // nready is the select function return value,indicate the already fd num
    // client[] indicate the client array
    int nready,client[FD_SETSIZE]; //FD_SETSIZE defined in sys/sokcet.h,value is 1024

    // Read function return value
    ssize_t n;
    
    //allset is the service initial fd_set
    //rset is the process fd_set,because select function can modify the fd_set,so need save the initial fd_set
    fd_set rset,all_set;

    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN]; //the outgoing parameter of inet_ntop,indicate the client ip str format

    struct sockaddr_in servaddr,cliaddr; //include ipv4 ipv6
    socklen_t cliaddr_len; //client's sockaddr_in actutual size(is ipv4 or ipv6)
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    Listen(listenfd,20);

    //inintialize the client array
    maxfd = listenfd; // current the max value file describe symol
    maxi = -1; // client array is empty

    for (i = 0; i < FD_SETSIZE; ++i)
        client[i] = -1; //client item not use
    FD_ZERO(&all_set);
    FD_SET(listenfd,&all_set);

    for( ;; ){
        rset = all_set;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0 )
            perr_exit("select error");
        
        if (FD_ISSET(listenfd, &rset)){ //When listenfd is set, it indicates that there is a new connection request on this socket that needs to be processed.
		    cliaddr_len = sizeof(cliaddr);
		    connfd = Accept(listenfd,(struct sockaddr *)&cliaddr, &cliaddr_len);
            printf("received from %s at PORT %d\n",
                        inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)) ? str : "unknown",
                        ntohs(cliaddr.sin_port));
            for (i = 0; i < FD_SETSIZE; i++)
                if(client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            if(i == FD_SETSIZE)
                perr_exit("too many clients");

            FD_SET(connfd,&all_set);

            if(connfd > maxfd)
                maxfd = connfd;
            if(i > maxi)
                maxi = i;
            if(--nready == 0)
                continue; 
        }

        for(i=0; i <= maxi; ++i){
            sockfd = client[i];
            if( sockfd < 0 )
                continue;
            if(FD_ISSET(sockfd, &rset)){
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
					/* connection closed by client */
					Close(sockfd);
					FD_CLR(sockfd, &all_set);
					client[i] = -1;
				} else {
					int j;
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					Write(sockfd, buf, n);
				}
				if (--nready == 0)
					break;	/* no more readable descriptors */
            }
        }

    }
    return 0;

}
