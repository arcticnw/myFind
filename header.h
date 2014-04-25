#ifndef _HEADER_H_
#define _HEADER_H_

#include <dirent.h>
#include <sys/stat.h>

struct condition_p;
union data_p {
	char * stringData;
	int intData;
	struct condition_p * conditionData;
};
typedef union data_p data_t;

struct file_p {
	struct dirent * dirEntry;
    char * localPath;
    char * realPath;
	struct stat dirEntryStat;
};
typedef struct file_p file_t;


struct condParams_p {
    char isCaseSensitive;
    signed char compareMethod;
};
typedef struct condParams_p condParams_t;


typedef int(* checker_t) (struct condition_p *, file_t);
typedef enum {INT, STRING, CONDITION, NONE} contents_t;

struct condition_p {
	checker_t doCheck;
    condParams_t params;

    contents_t data1content;
	data_t data1;

    contents_t data2content;
	data_t data2;
};
typedef struct condition_p condition_t;

struct action_p;
typedef void(* processer_t) (struct action_p *, file_t);

struct action_p
{
    processer_t doProcess;
    int paramsCount;
    char ** params;
    struct action_p * next;
};
typedef struct action_p action_t;

struct argsBundle_p {
    condition_t * condition;
    action_t * action;
    char * path;
    char followLinks;
    char ignoreHidden;
};
typedef struct argsBundle_p argsBundle_t;

#endif