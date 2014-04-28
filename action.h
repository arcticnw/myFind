#ifndef _CRAWLER_H_
#define	_CRAWLER_H_

#include "common.h"

/* this pointer specifies action action_p that will be applied on             */
/*     file_t that pass the check test                                        */
typedef void(* act_t) (struct action_p *, file_t);


/* this structure holds information about action:                             */
/*     the action function, it's parameters (and count),                      */
/*     and a pointer to next action                                           */
struct action_p {
    act_t do_action;
    int param_count;
    char **params;
    struct action_p * next;
};
typedef struct action_p action_t;


/* this function prints the filename and its relative path on standard output */
void 
do_print(action_t *action, file_t file);


/* this function executes a program with given arguments and filename with its*/
/*     relative path                                                          */
void 
do_execute(action_t *action, file_t file);

#endif