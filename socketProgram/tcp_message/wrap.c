#include "./include/wrap.h"
void perr_exit(const char *s){
    perror(s);
    exit(1);
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr){
    int n; //返回文件描述符
again:
    if ((n = accept(fd, sa, salenptr)) < 0){ 
        if (errno == ECONNABORTED || errno == ECONNRESET || errno == ETIMEDOUT || errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            goto again;
        else
            perr_exit("accept error");
    }
    return n;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen){
    if (bind(fd, sa, salen) < 0)
        perr_exit("bind error");
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen){
    if (connect(fd, sa, salen) < 0)
        perr_exit("connect error");
}

void Listen(int fd, int backlog){
    if (listen(fd, backlog) < 0)
        perr_exit("listen error");
}

int Socket(int family, int type, int protocol){
    int n;
    if ((n = socket(family, type, protocol)) < 0)
        perr_exit("socket error");
    return n;
}

ssize_t Read(int fd, void *ptr, size_t nbytes){
    ssize_t n;
again:
    if ((n = read(fd, ptr, nbytes)) == -1){
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            goto again;
        else
            return -1;
    }
    return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes){
    ssize_t n;
again:
    if ((n = write(fd, ptr, nbytes)) == -1){
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            goto again;
        else
            return -1;
    }
    return n;
}

void Close(int fd){
    if (close(fd) == -1)
        perr_exit("close error");
}

ssize_t Readn(int fd, void *vptr, size_t n){
	size_t  nleft; //request read size
	ssize_t nread; //call Read return value
	char   *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0){
		if ((nread = Read(fd, ptr, nleft))< 0){ //Read can reread
				return -1;
		}else if (nread == 0)
			break;
		nleft -= nread; //still need read how many bytes
		ptr += nread;
	}
	return n - nleft; // actually read size
}

ssize_t Writen(int fd, const void *vptr, size_t n){
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0){
		if ((nwritten = write(fd, ptr, nleft)) <= 0)
				return -1;
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

static ssize_t my_read(int fd, char *ptr){
	static int read_cnt; //read byte num
	static char *read_ptr; //current read character pointer
	static char read_buf[100];//read buffer

	if (read_cnt <= 0){
		if ((read_cnt = Readn(fd, read_buf, sizeof(read_buf))) < 0){
			return -1;
		}else if(read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}
	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t Readline(int fd, void *vptr, size_t maxlen){
	ssize_t n, rc;
	char    c, *ptr;
	ptr = vptr;
	for (n = 1; n < maxlen; n++){
		if ((rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c  == '\n')
				break;
		}else if (rc == 0) {
			*ptr = 0;
			return n - 1;
		}else
			return -1;
	}
	*ptr  = 0;
	return n;
}

void proStringSpace(char * str){
    // 去掉末尾的空白字符（包括换行符、空格、制表符等）
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '@' || str[len - 1] == '\003' || str[len -1 ] == '\001')) {
        str[--len] = '\0';
    }
}
