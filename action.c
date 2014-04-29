#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#include "common.h"
#include "action.h"

void do_print(action_t *action, file_info_bundle_t file) {
	printf("%s\n", file.local_path);
}

void do_execute(action_t *action, file_info_bundle_t file) {
	pid_t child_pid;
	pid_t wait_pid;
	int i;

	child_pid = fork();

	switch(child_pid) {
		case -1:
			errx(127, FORK_ERR_MSG, strerror(errno));
			break;
		case 0:
			/* this is child process */

			/* find the replacement token and replace it */
			for (i = 0; i < action->param_count; i++) {
				if (!strcmp(action->params[i], EXEC_REPLACE_TOKEN)) {
					/* replace the token */
					free(action->params[i]);
					action->params[i] = file.local_path;
				}
			}

			/* execute the command with given arguments */
			execvp(action->params[0], action->params);
			/* if program happens to reach this point, exec failed */
			errx(127, EXEC_ERR_MSG, strerror(errno));
			break;
		default:
			/* this is parent process */

			/* wait for the child before continuiing */
			wait_pid = waitpid(child_pid, NULL, 0);
			if (wait_pid != child_pid) {
				/* ignore */
			}
			break;
	}
}