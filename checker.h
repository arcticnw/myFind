#ifndef _CHECKER_H_
#define	_CHECKER_H_

#include "common.h"

/* this function returns 1                                                    */
int 
check_true(condition_t *condition, file_t file);


/* this function returns 0                                                    */
int 
check_false(condition_t *condition, file_t file);


/* this function applies do_check function on the condition                   */
int 
check(condition_t *condition, file_t file);


/* this function negates the return value of a condition                      */
int 
check_not(condition_t *condition, file_t file);


/* this function applies AND operator on the results of the two conditions    */
int 
check_and(condition_t *condition, file_t file);


/* this function applies OR operator on the results of the two conditions     */
int 
check_or(condition_t *condition, file_t file);


/* this function tries to match file name with provided pattern               */
int 
check_name(condition_t *condition, file_t file);


/* this function changes all characters in the literal to lower case          */
void 
string_to_lower(char *data);

#endif