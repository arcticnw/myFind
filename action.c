#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"
#include "action.h"

void do_print(action_t *action, file_t file)
{
	printf("%s\n", file.local_path);
}

void do_execute(action_t *action, file_t file)
{
	pid_t child_pid;
	pid_t wait_pid;
	int execerr;
	
	child_pid = fork();
	
	switch(child_pid)
	{
		case -1:
			errx(127, "Failed to fork: %s", strerror(errno));
			break;
		case 0:
			/* this is child process */
			execerr = execvp(action->params[0], action->params]);
			errx(127, "Failed to exec: %s", strerror(errno));
			break;
		default:
			/* this is parent process */
			wait_pid = waitpid(child_pid, NULL, 0);
			if (wait_pid != child_pid)
			{
				assert(0);
				/* ignore */
			}
			break;
	}
}