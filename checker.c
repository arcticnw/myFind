#include <err.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <fnmatch.h>
#include <stdlib.h>

#include "common.h"
#include "checker.h"

int check_true(condition_t *condition, file_t file)
{
    return (1);
}
int check_false(condition_t *condition, file_t file)
{
    return (0);
}

int check(condition_t *condition, file_t file)
{
    assert(condition);
    assert(condition->do_check);

    return (condition->do_check(condition, file));
}
int check_not(condition_t *condition, file_t file)
{
    int result = 0;
    
    assert(condition);
    assert(condition->data1_content == CONDITION);
    assert(condition->data1.condition_data);

    result = check(condition->data1.condition_data, file);
    
    return (!result);
}

int check_and(condition_t *condition, file_t file)
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
int check_or(condition_t *condition, file_t file)
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

int check_name(condition_t *condition, file_t file)
{
    char *file_name;
    int return_value;
    
    assert(condition);
    assert(condition->data1_content == STRING);
	assert(condition->data1.string_data);
    
    if (!condition->params.is_case_sensitive)
    {
		file_name = copy_string(file.dir_entry->d_name);
        string_to_lower(file_name);
    }
    else
    {
        file_name = file.dir_entry->d_name;
    }
    
    return_value = !fnmatch(condition->data1.string_data, file_name, FNM_PATHNAME);
    
    if (!condition->params.is_case_sensitive)
    {
        free(file_name);
    }
    
    return (return_value);
}