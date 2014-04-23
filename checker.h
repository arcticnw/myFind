#ifndef _CHECKER_H_
#define _CHECKER_H_

#include "header.h"

int checkTrue(data_t *data1, data_t *data2, file_tfile);
int checkFalse(data_t *data1, data_t *data2, file_tfile);

int checkAnd(data_t *data1, data_t *data2, file_tfile);
int checkOr(data_t *data1, data_t *data2, file_tfile);

int checkName(data_t *data1, data_t *data2, file_tfile);

#endif