HEADER = include/
CC = gcc
CFLAGS = -g -I$(HEADER) 

compile: service client

service: service.c wrap.c
	$(CC) $(CFLAGS) -o $@ $^

client: client.c wrap.c
	$(CC) $(CFLAGS) -o $@ $^

tags:
	ctags -R .

.PHONY : clean
clean: 
	-rm service client tags
