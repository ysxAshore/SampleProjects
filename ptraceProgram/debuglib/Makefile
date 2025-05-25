DEFAULT_CFLAGS += -Wall -Werror -o 
NOPIE_CFLAGS = -static -no-pie $(DEFAULT_CFLAGS)
ASM_CFLAGS = -nostartfiles -nostdlib $(NOPIE_CFLAGS)
INCLUDE_FILES = ./include

default : all

all: debugger c_program

debugger: bp_use_lib.c debuglib.c
	$(CC) -I$(INCLUDE_FILES) $(DEFAULT_CFLAGS) $@ $^

c_program: tracee.c
	$(CC) $(NOPIE_CFLAGS) $@ $^

testC: debugger c_program
	@./debugger ./c_program $(BREAKADDR)

.PHONY: all clean
clean: 
	-rm debugger c_program

