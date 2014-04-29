#ifndef _ACTION_H_
#define	_ACTION_H_

#include "common.h"

#define	FORK_ERR_MSG "Failed to fork: %s"
#define	EXEC_ERR_MSG "Failed to exec: %s"
#define	EXEC_REPLACE_TOKEN "{}"

/* this function prints the filename and its relative path on standard output */
void
do_print(action_t *action, file_info_bundle_t file);


/* this function executes a program with given arguments and filename with its*/
/* relative path */
void
do_execute(action_t *action, file_info_bundle_t file);

#endif