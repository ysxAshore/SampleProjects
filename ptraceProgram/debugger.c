#include <stdio.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

void run_target(const char * programName){
    printf("target started. will run '%s'\n", programName);
//	raise(SIGSTOP);
    /* Replace this process's image with the given program */
    execl(programName, programName, NULL);	
}

void run_debugger(pid_t child_pid){
  int wait_status;
    unsigned icounter = 0;
    printf("debugger started\n");
	
	if(ptrace(PTRACE_ATTACH,child_pid,0,0)<0)
		perror("attach");
    /* Wait for child to stop on its first instruction */
    waitpid(child_pid,&wait_status,0);

    while (WIFSTOPPED(wait_status)) {
        icounter++;
		struct user_regs_struct regs;
		ptrace(PTRACE_GETREGS,child_pid,0,&regs);
		unsigned inst = ptrace(PTRACE_PEEKTEXT,child_pid,regs.rip,0);
		
        printf("icounter = %u.  RIP = 0x%08llx.  inst = 0x%08x\n",
                    icounter, regs.rip, inst);

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
            perror("ptrace");
            return;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    printf("the child executed %u instructions\n", icounter);
}

int main(int argc, char** argv)
{
    pid_t child_pid;

    if (argc < 2) {
        fprintf(stderr, "Expected a program name as argument\n");
        return -1;
    }

    child_pid = fork();
    if (child_pid == 0)
        run_target(argv[1]);
    else if (child_pid > 0)
        run_debugger(child_pid);
    else {
        perror("fork");
        return -1;
    }

    return 0;
}
