#ifndef _COMMON_H_
#define	_COMMON_H_

#include <dirent.h>
#include <sys/stat.h>

struct condition_p;
struct action_p;

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