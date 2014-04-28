#ifndef _COMMON_H_
#define	_COMMON_H_

#include <dirent.h>
#include <sys/stat.h>

/* FILE RELATED STRUCTURES */

/* this structure holds information about currently processed file            */
struct file_p {
	struct dirent * dir_entry;
    char *local_path;
    char *real_path;
	struct stat dir_entry_stat;
};
typedef struct file_p file_t;


/* ACTION RELATED STRUCTURES */

struct action_p;

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


/* CONDITION RELATED STRUCTURES */

struct condition_p;

/* this structure holds data for one side of an operator/check function       */
union data_p {
	char *string_data;
	int int_data;
	struct condition_p * condition_data;
};
typedef union data_p data_t;


/* this enum specifies the type of content present in the data_p              */
typedef enum {INT, STRING, CONDITION, NONE} content_t;


/* this structure holds additional parameters for a check function            */
struct cond_params_p {
    char is_case_sensitive;
    signed char compare_method;
};
typedef struct cond_params_p cond_params_t;


/* this pointer specifies operator/check function that determines             */
/*     whether file_t satisfies target condition */
typedef int(* check_t) (struct condition_p *, file_t);


/* this structure holds information about condition:                          */
/*     the operator/check function, its parameters, and two operands          */
struct condition_p {
	check_t do_check;
    cond_params_t params;

    content_t data1_content;
	data_t data1;

    content_t data2_content;
	data_t data2;
};
typedef struct condition_p condition_t;

/* */

/* this structure holds informations parsed from the arguments of the program */
struct args_bundle_p {
    char *path;
    condition_t *condition;
    action_t *action;
    char follow_links;
    char ignore_hidden;
};
typedef struct args_bundle_p args_bundle_t;

#endif