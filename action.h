#ifndef _CRAWLER_H_
#define	_CRAWLER_H_

#include "common.h"

/* this function prints the filename and its relative path on standard output */
void 
do_print(action_t *action, file_t file);


/* this function executes a program with given arguments and filename with its*/
/*     relative path                                                          */
void 
do_execute(action_t *action, file_t file);

#endif