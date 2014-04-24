#ifndef _CHECKER_H_
#define _CHECKER_H_

#include "header.h"

int checkTrue(condition_t * condition, file_t file);
int checkFalse(condition_t * condition, file_t file);

int check(condition_t * condition, file_t file);
int checkNot(condition_t * condition, file_t file);
int checkAnd(condition_t * condition, file_t file);
int checkOr(condition_t * condition, file_t file);

int checkName(condition_t * condition, file_t file);


void tolowerarray(char * data);

#endif