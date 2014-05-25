#ifndef _CHECKER_IMPL_H_
#define	_CHECKER_IMPL_H_

#include "common.h"

/* this function computes the difference of the time since file_time until */
/* now divides this time by accurancy and then compares it to target */
/* time using specified comparison method */
static int
compare_time(time_t now, time_t file_time, long accurancy, long target,
    char compare_method);

#endif