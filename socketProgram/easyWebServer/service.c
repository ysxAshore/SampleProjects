#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../tcp_message/include/wrap.h"

#define MAXLINE 256 //Buffer Size
int SERV_PORT;
char dirname[80];

void readConfig(){
    FILE * configFile = fopen("myhttpd.conf","r");
    if (configFile == NULL)
        perr_exit("/etc/myhttpd.conf not exists");
    
    char buf[MAXLINE],port[6];
    char * ptr;    
    
    //read Port=80
    fgets(buf,MAXLINE,configFile);
    proStringSpace(buf); 
    ptr = strchr(buf,'=');
    if(ptr == NULL)
        perr_exit("config not have port");
    strncpy(port, ptr + 1, sizeof(port) - 1);
    port[sizeof(port) - 1] = '\0'; // 确保字符串以null结尾
    proStringSpace(port);
    SERV_PORT = atoi(port);
    
    //read Directory=/var/www
    fgets(buf,MAXLINE,configFile);
    proStringSpace(buf);
    ptr = strchr(buf,'=');
    if(ptr == NULL)
        perr_exit("config not have dirname");
    strncpy(dirname, ptr + 1, sizeof(dirname) - 1);
    proStringSpace(dirname);
    dirname[sizeof(dirname) - 1] = '\0';

}

int isExistFile(char * filename) {
    /*
    not exist : return 0
    exist but not exec : return 1
    exist and exec: retun 2
    */
    struct stat buffer;
    
    char full_path[MAXLINE];
    snprintf(full_path, sizeof(full_path), "%s%s", dirname, filename);

    if (stat(full_path,&buffer) != 0)
        return 0;
    else if(buffer.st_mode & S_IXUSR || buffer.st_mode & S_IXGRP || buffer.st_mode & S_IXOTH)
        return 2;
    else
        return 1;
}

// 根据文件扩展名获取Content-Type
const char* get_content_type(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (ext == NULL) return "text/html";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ". htm") == 0) {
        return "text/html";
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else {
        return "text/html";
    }
}

// 发送HTTP响应头
void send_response_header(int client_socket, const char* content_type,int state) {
    char header[256] = {'\0'};
    if(state == 1){
        snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n" \
             "Content-Type: %s\r\n" \
             "\r\n", content_type);
    }else if(state == 2){
        strcpy(header,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");  
    }
    else{
        strcpy(header,"HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"); 
    }
    Write(client_socket, header, strlen(header));
}

// 发送文件内容
void send_file_content(int client_socket, const char* filename,int state) {
    char buffer[MAXLINE] = {'\0'};
    char full_path[MAXLINE];
    snprintf(full_path, sizeof(full_path), "%s%s", dirname, filename);
    
    if(state == 0){
        strcpy(buffer,"<html><body>request file not found</body></html>\r\n");
        Write(client_socket,buffer,strlen(buffer));
    }else if(state == 1){
        FILE* file = fopen(full_path, "rb");
        if (file == NULL) {
            perr_exit("fopen");
            return;
        } 

        char buffer[MAXLINE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, MAXLINE, file)) > 0) {
            Write(client_socket, buffer, bytes_read);
        } 

        fclose(file);
    }else{
        int pid = fork();
        if(pid == 0){
            dup2(client_socket,STDOUT_FILENO);
            execlp(full_path,filename,NULL);
            perr_exit("exec");
        } 
    }
}

int main(int argc,char *argv[]) {

    readConfig();

    int i ,maxi, maxfd,listenfd,connfd,sockfd;
     
    int nready,client[FD_SETSIZE]; 
    
    ssize_t n;
    
    fd_set rset,all_set;

    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN]; 

    struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddr_len; 
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    
    Listen(listenfd,20);

    maxfd = listenfd; 
    maxi = -1;

    for (i = 0; i < FD_SETSIZE; ++i)
        client[i] = -1;
    FD_ZERO(&all_set);
    FD_SET(listenfd,&all_set);

    for( ;; ){
        rset = all_set;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0 )
            perr_exit("select error");
        
        if (FD_ISSET(listenfd, &rset)){ 
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

        for(i = 0; i <= maxi; ++i){
            sockfd = client[i];
            if( sockfd < 0 )
                continue;
            if(FD_ISSET(sockfd, &rset)){
                    if(Read(sockfd,buf,MAXLINE) != 0){
                        char filepath[MAXLINE];
                        char * ptr;
                        ptr = strchr(buf,'/');
                        if (ptr == NULL)
                            perr_exit("Error Request");
                        strncpy(filepath,ptr,sizeof(filepath)-1);
                        filepath[sizeof(filepath)-1] = '\0';
                        proStringSpace(filepath);
                        for(int i = 0; i < strlen(filepath); ++i){
                            if(filepath[i] == ' '){
                                filepath[i] = '\0';
                                break;
                            }
                        }
                        
                        int rval = isExistFile(filepath);
                        
                        send_response_header(sockfd,get_content_type(filepath),rval);

                        send_file_content(sockfd,filepath,rval);
                    }
					Close(sockfd);
					FD_CLR(sockfd, &all_set);
					client[i] = -1;
				
				if (--nready == 0)
					break;	
            }
        }

    }


    return 0;
}

