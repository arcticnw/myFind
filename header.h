#ifndef _HEADER_H_
#define _HEADER_H_

#include <dirent.h>
#include <sys/stat.h>

struct condition_p;

typedef enum {INT, STRING, CONDITION, NONE} contents_t;

union data_p {
	char* stringData;
	int intData;
	struct condition_p * conditionData;
};
typedef union data_p data_t;

struct file_p {
	struct dirent *dirEntry;
    char * localPath;
    char * realPath;
	struct stat d_entry_stat;
};
typedef struct file_p file_t;

typedef int(*processer_t) (data_t *, data_t *, file_t);

struct condition_p {
	processer_t process;
	data_t * data1;
    contents_t data1content;
	data_t * data2;
    contents_t data2content;
};
typedef struct condition_p condition_t;

#endif