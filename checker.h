#ifndef _CHECKER_H_
#define	_CHECKER_H_

#include "common.h"

/* this function returns 1 */
int
check_true(condition_t *condition, file_info_bundle_t file);


/* this function returns 0 */
int
check_false(condition_t *condition, file_info_bundle_t file);


/* this function applies do_check function on the condition */
int
check(condition_t *condition, file_info_bundle_t file);


/* this function negates the return value of a condition */
int
check_not(condition_t *condition, file_info_bundle_t file);


/* this function applies AND operator on the results of the two conditions */
int
check_and(condition_t *condition, file_info_bundle_t file);


/* this function applies OR operator on the results of the two conditions */
int
check_or(condition_t *condition, file_info_bundle_t file);


/* this function tries to match file name with provided pattern */
int
check_name(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was accessed specified time ago */
int check_atime(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was modified specified time ago */
int check_mtime(condition_t *condition, file_info_bundle_t file);


/* this function determines if the file was created specified time ago */
int check_ctime(condition_t *condition, file_info_bundle_t file);


/* this function tests if regular file or directory is empty */
int
check_empty(condition_t *condition, file_info_bundle_t file);


/* this function checks the file's group id */
int
check_gid(condition_t *condition, file_info_bundle_t file);


/* this function checks the file's user id */
int
check_uid(condition_t *condition, file_info_bundle_t file);


/* this function checks is the file size is of the specified size */
int
check_size(condition_t *condition, file_info_bundle_t file);

#endif