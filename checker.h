#ifndef _CHECKER_H_
#define	_CHECKER_H_

#include "common.h"

/* this function returns 1                                                    */
int 
check_true(condition_t *condition, file_info_bundle_t file);


/* this function returns 0                                                    */
int 
check_false(condition_t *condition, file_info_bundle_t file);


/* this function applies do_check function on the condition                   */
int 
check(condition_t *condition, file_info_bundle_t file);


/* this function negates the return value of a condition                      */
int 
check_not(condition_t *condition, file_info_bundle_t file);


/* this function applies AND operator on the results of the two conditions    */
int 
check_and(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was accessed specified time ago       */
int check_atime(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was modified specified time ago       */
int check_mtime(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was created specified time ago        */
int check_ctime(condition_t *condition, file_info_bundle_t file);


/* this function applies OR operator on the results of the two conditions     */
int 
check_or(condition_t *condition, file_info_bundle_t file);


/* this function tries to match file name with provided pattern               */
int 
check_name(condition_t *condition, file_info_bundle_t file);


/* this function computes the difference of the time since file_time until now*/
/*     divides this time by accurancy and then compares it to target timeframe*/
/*     using specified comparison method                                      */
int compare_time(time_t now, time_t file_time, long long accurancy, long long target, char compare_method);
#endif