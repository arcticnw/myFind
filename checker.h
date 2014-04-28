#ifndef _CHECKER_H_
#define	_CHECKER_H_

#include "common.h"

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
typedef int(* chech_t) (struct condition_p *, file_t);


/* this structure holds information about condition:                          */
/*     the operator/check function, its parameters, and two operands          */
struct condition_p {
	chech_t do_check;
    cond_params_t params;

    content_t data1_content;
	data_t data1;

    content_t data2_content;
	data_t data2;
};
typedef struct condition_p condition_t;


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