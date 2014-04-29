#include <stdio.h>
#include <err.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "checker.h"

int check_true(condition_t *condition, file_info_bundle_t file)
{
    return (1);
}
int check_false(condition_t *condition, file_info_bundle_t file)
{
    return (0);
}

int check(condition_t *condition, file_info_bundle_t file)
{
    assert(condition);
    assert(condition->do_check);

    return (condition->do_check(condition, file));
}
int check_not(condition_t *condition, file_info_bundle_t file)
{
    int result = 0;
    
    assert(condition);
    assert(condition->data1_content == CONDITION);
    assert(condition->data1.condition_data);

    result = check(condition->data1.condition_data, file);
    
    return (!result);
}

int check_and(condition_t *condition, file_info_bundle_t file)
{
    int leftSideResult = 0;
    int rightSideResult = 0;
    
    assert(condition);
    assert(condition->data1_content == CONDITION);
	assert(condition->data1.condition_data);
    assert(condition->data2_content == CONDITION);
    assert(condition->data2.condition_data);
    
    leftSideResult = check(condition->data1.condition_data, file);
	
	/* short circuited evaluation */
	if (!leftSideResult)
	{
		return (leftSideResult);
	}
	
    rightSideResult = check(condition->data2.condition_data, file);
      
    return (leftSideResult && rightSideResult);
}
int check_or(condition_t *condition, file_info_bundle_t file)
{
    int leftSideResult = 0;
    int rightSideResult = 0;

    assert(condition);
    assert(condition->data1_content == CONDITION);
	assert(condition->data1.condition_data);
    assert(condition->data2_content == CONDITION);
    assert(condition->data2.condition_data);
	
    leftSideResult = check(condition->data1.condition_data, file);
	
	/* short circuited evaluation */
	if (leftSideResult)
	{
		return (leftSideResult);
	}
	
    rightSideResult = check(condition->data2.condition_data, file);
      
    return (leftSideResult || rightSideResult);
}

int check_name(condition_t *condition, file_info_bundle_t file)
{
	char *file_name;
	int return_value;

	assert(condition);
	assert(condition->data1_content == STRING);
	assert(condition->data1.string_data);
	/* data1 = pattern */

	if (!condition->params.is_case_sensitive)
	{
		file_name = copy_string(file.file_entry->d_name);
		string_to_lower(file_name);
	}
	else
	{
		file_name = file.file_entry->d_name;
	}

	return_value = !fnmatch(condition->data1.string_data, file_name, FNM_PATHNAME);

	if (!condition->params.is_case_sensitive)
	{
		free(file_name);
	}

	return (return_value);
}

int check_atime(condition_t *condition, file_info_bundle_t file)
{
	assert(condition);
	assert(condition->data1_content == LONG);
	assert(condition->data2_content == LONG);
	/* data1 = compared time */
	/* data2 = accurancy (divisor) */
	
	return ( compare_time(
	    file.time_now, 
	    file.file_entry_stat.st_atime, 
	    condition->data2.long_data, 
	    condition->data1.long_data,
	    condition->params.compare_method) );
}

int check_mtime(condition_t *condition, file_info_bundle_t file)
{
	assert(condition);
	assert(condition->data1_content == LONG);
	assert(condition->data2_content == LONG);
	/* data1 = compared time */
	/* data2 = accurancy (divisor) */
	
	return ( compare_time(
	    file.time_now, 
	    file.file_entry_stat.st_mtime, 
	    condition->data2.long_data, 
	    condition->data1.long_data,
	    condition->params.compare_method) );
}

int check_ctime(condition_t *condition, file_info_bundle_t file)
{
	assert(condition);
	assert(condition->data1_content == LONG);
	assert(condition->data2_content == LONG);
	/* data1 = compared time */
	/* data2 = accurancy (divisor) */
	
	return ( compare_time(
	    file.time_now, 
	    file.file_entry_stat.st_ctime, 
	    condition->data2.long_data, 
	    condition->data1.long_data,
	    condition->params.compare_method) );
}

int check_empty(condition_t *condition, file_info_bundle_t file)
{
	DIR * dir;
	int n;

	/* is empty regular file */
	if (S_ISREG(file.file_entry_stat.st_mode) && !file.file_entry_stat.st_size)
	{
		return (1);
	}
	
	/* is empty directory */
	if (S_ISDIR(file.file_entry_stat.st_mode))
	{
		n = 0;
		dir = opendir(file.file_entry->d_name);
		if (dir)
		{
			while (readdir(dir)) 
			{
				n++;
				if (n > 2)
				{
					break;
				}
			}
			closedir(dir);
		}
		if (n == 2)
		{
			return (1);
		}
	}
	
	return (0);
}

int check_gid(condition_t *condition, file_info_bundle_t file)
{
	assert(condition);
	assert(condition->data1_content == LONG);
	
	return (file.file_entry_stat.st_gid == condition->data1.long_data);
}

int check_uid(condition_t *condition, file_info_bundle_t file)
{
	assert(condition);
	assert(condition->data1_content == LONG);
	
	return (file.file_entry_stat.st_uid == condition->data1.long_data);
}

int check_size(condition_t *condition, file_info_bundle_t file)
{
	long file_size;
	long target;

	assert(condition);
	assert(condition->data1_content == LONG);
	
	file_size = file.file_entry_stat.st_size;
	target = condition->data1.long_data;
	
	switch (condition->params.compare_method)
	{
		case '-': 
			return file_size < target; 
			break;
		case '+':
			return file_size > target; 
			break;
		default:
			return file_size = target; 
			break;
	}
	
	return (file.file_entry_stat.st_gid == condition->data1.long_data);
}

int compare_time(time_t now, time_t file_time, long accurancy, long target, char compare_method)
{
	long difference;

	assert(accurancy);
	if (!accurancy)
	{
		accurancy = 1;
	}

	difference = (now - file_time) / accurancy;

	switch (compare_method)
	{
		case '-': 
			return difference < target; 
			break;
		case '+':
			return difference > target; 
			break;
		default:
			return difference = target; 
			break;
	}
}