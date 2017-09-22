//Homework for 23/09/2017 by Pashkovskiy Alexander, 627 group

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define COMAND_SIZE 8192

int main()
{
	pid_t pid = 1;
	int wait_status;

	char cmd[COMAND_SIZE];

	strcpy(cmd, "Print 'exit' to exit the program\n");
	printf("%s\n", cmd);

	fgets(cmd, COMAND_SIZE, stdin);
	while (strncmp(cmd, "exit", 4) || !isspace(cmd[4])) {

		pid = fork();
		if (pid < 0) 
			perror("unenable to fork\n");
		else {
			if (pid == 0) {
// parsing cmd
				char* parsed_cmd[COMAND_SIZE];
				parsed_cmd[0] = cmd;
				int parsed_cmd_counter = 0, position;

				for(position = 1; cmd[position] != '\0'; position++) {
					if (isspace(cmd[position])) {
						cmd[position] = '\0';
						if (!isspace(cmd[position+1]))
							parsed_cmd[++parsed_cmd_counter] = cmd + position +1;
					}
				}

				parsed_cmd[parsed_cmd_counter] = (char*) NULL;
// end of parsing cmd
// cannot to write parsing as a function because it demands using shared memory
				execvp(parsed_cmd[0], parsed_cmd);
				perror("An error during comand execution");
				exit(EXIT_FAILURE);
			}
			else{
				if (waitpid(pid, &wait_status, 0) < 0)
					perror("A problem with cild process");
				else {
					if (WIFEXITED(wait_status))
						printf("Child process terminated correclry. Exit status: %d\n", WEXITSTATUS(wait_status));
					else
						printf("Child process has been enterrupted\n");
				}

				printf("\n");
			}
		}
		fgets(cmd, COMAND_SIZE, stdin);
	}
	return 0;
}
