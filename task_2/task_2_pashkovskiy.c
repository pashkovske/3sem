#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define STR_SIZE 256

typedef struct {
	int txd[2];	// parent wtites
	int rxd[2];	// child writes
} dpipe_t;

int init_duplex_pipe(dpipe_t* duplex_pipe)
{
	if (pipe(duplex_pipe -> txd) == -1) {
		perror("pipe");
		return 1;
	}
	if (pipe(duplex_pipe -> rxd) == -1) {
		perror("pipe");
		return 1;
	}
	return 0;
}

int main()
{
	int length, i;
	pid_t cpid;
	char buf;
	char child_str[STR_SIZE], parent_str[STR_SIZE];
	
	dpipe_t useless;
	if (init_duplex_pipe(&useless)) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {
		close(useless.txd[1]);	// closing writing descriptor in pipe where parent writes
		close(useless.rxd[0]);	// closing reading descriptor in pipe where child writes

		strcpy(child_str, "I am child, lalala...");
		write(useless.rxd[1], child_str, strlen(child_str)+1);
		printf ("I am child, my pid is %d, I,ve sent some information to duplex pipe.\n", getpid());

		i = 0;
		do {
			read(useless.txd[0], &buf, 1);
			parent_str[i++] = buf;
		} while (buf != '\0');
		printf ("I am child, my pid is %d, I,ve recived some information from duplex pipe:\n\t%s\n", getpid(), parent_str);

		close(useless.txd[0]);	// closing all the descriptor
		close(useless.rxd[1]);

		exit(EXIT_SUCCESS);
	} else {
		close(useless.txd[0]);	// closing reading descriptor in pipe where parent writes
		close(useless.rxd[1]);	// closing writing descriptor in pipe where child writes

		strcpy(parent_str, "I am the father, MUHAHAHA!");
		write(useless.txd[1], parent_str, strlen(parent_str)+1);
		printf ("I am parent, my pid is %d, I,ve sent some information to duplex pipe.\n", getpid());

		i = 0;
		do {
			read(useless.rxd[0], &buf, 1);
			child_str[i++] = buf;
		} while (buf != '\0');
		printf ("I am parent, my pid is %d, I,ve recived some information from duplex pipe:\n\t%s\n", getpid(), child_str);

		close(useless.txd[1]);	// closing all the descriptor
		close(useless.rxd[0]);

		wait(NULL);

		exit(EXIT_SUCCESS);
	}
}
