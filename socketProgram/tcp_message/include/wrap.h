#include <stdlib.h>
#include <error.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

/*打印错误信息并exit*/
void perr_exit(const char *s);

/*封装accept函数 根据errno决定是否重发*/
int Accept(int fd, struct sockaddr *sa, socklen_t * salenptr);

/*封装bind函数*/
void Bind(int fd,const struct sockaddr *sa, socklen_t salen);

/*封装connect函数*/
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

/*封装listen函数*/
void Listen(int fd, int backlog);

/*封装socket函数*/
int Socket(int family, int type, int protocol);

/*封装read函数*/
ssize_t Read(int fd, void *ptr, size_t nbytes);

/*封装write函数*/
ssize_t Write(int fd, const void *ptr, size_t nbytes);

/*封装close函数*/
void Close(int fd);

/*多次调用Read确保请求指定的n字节*/
ssize_t Readn(int fd,void *vptr,size_t n);

/*多次调用Write确保写指定字节*/
ssize_t Writen(int fd,const void *vptr,size_t n);

/*Readline 类似fgets的功能*/
/*从文件描述符 fd 中读取一个缓冲区，每次从缓冲区中读入一个字符并将其存入指针 ptr 所指向的内存位置,缓冲区读完时重新从文件中读取新的缓冲区*/
static ssize_t my_read(int fd, char *ptr);
/*从文件描述符 fd 中读取一行数据，直到遇到换行符 \n 或达到最大长度 maxlen*/
ssize_t Readline(int fd, void *vptr, size_t maxlen);

void proStringSpace(char * str);
