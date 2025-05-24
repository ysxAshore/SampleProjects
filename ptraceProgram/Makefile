DEFAULT_CFLAGS += -Wall -Werror -o 
ASM_CFLAGS = -nostartfiles -nostdlib -static $(DEFAULT_CFLAGS)

default : all

all: debugger c_program asm_program

debugger: debugger.c
	$(CC) $(DEFAULT_CFLAGS) $@ $^

c_program: trace_program.c
	$(CC) $(DEFAULT_CFLAGS) $@ $^

asm_program: hello.s
	$(CC) $(ASM_CFLAGS) $@ $^

testC: debugger c_program
	@./debugger ./c_program
testS: debugger asm_program
	@./debugger ./asm_program

.PHONY: all clean
clean: 
	-rm debugger c_program asm_program

