#ifndef _HEADER_H_
#define _HEADER_H_

#include <dirent.h>
#include <sys/stat.h>

struct condition_p;

union data_p {
	char* stringData;
	int intData;
	struct condition_p * p;
};
typedef union data_p data_t;

struct file_p {
	struct dirent *d_entry;
	int d_entry_stat_filled;
	struct stat d_entry_stat;
};
typedef struct file_p file_t;

struct condition_p {
	int (*process)(data_t *, data_t *, file_t *);
	data_t * data1;
	data_t * data2;
};
typedef struct condition_p condition_t;

#endif