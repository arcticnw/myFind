#include <err.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <fnmatch.h>

#include "header.h"
#include "checker.h"

int checkTrue(condition_t * condition, file_t file)
{
    return (1);
}
int checkFalse(condition_t * condition, file_t file)
{
    return (0);
}

int check(condition_t * condition, file_t file)
{
    assert(condition);
    assert(condition->process);

    return (condition->process(condition, file));
}

int checkNot(condition_t * condition, file_t file)
{
    int result = 0;
    
    assert(condition);
    assert(condition->data1content == CONDITION);

    result = check(condition->data1.conditionData, file);
    
    return (!result);
}

int checkAnd(condition_t * condition, file_t file)
{
    int leftSideResult = 0;
    int rightSideResult = 0;
    
    assert(condition);
    assert(condition->data1content == CONDITION);
    assert(condition->data2content == CONDITION);
    
    leftSideResult = check(condition->data1.conditionData, file);
    rightSideResult = check(condition->data2.conditionData, file);
      
    return (leftSideResult && rightSideResult);
}
int checkOr(condition_t * condition, file_t file)
{
    int leftSideResult = 0;
    int rightSideResult = 0;

    assert(condition);
    assert(condition->data1content == CONDITION);
    assert(condition->data2content == CONDITION);
    
    leftSideResult = check(condition->data1.conditionData, file);
    rightSideResult = check(condition->data2.conditionData, file);
      
    return (leftSideResult || rightSideResult);
}

int checkName(condition_t * condition, file_t file)
{
    char * fileName;
    int returnValue;
    
    assert(condition);
    assert(condition->data1content == STRING);
    
    if (condition->params.caseSensitivity == INSENSITIVE)
    {
        fileName = (char*) malloc(sizeof(char) * strlen(file_t.dirEntry->d_name));
        strcpy(fileName, file_t.dirEntry->d_name);
        tolowerarray(fileName);
    }
    else
    {
        fileName = file_t.dirEntry->d_name;
    }
    
    returnValue = !fnmatch(condition->data1.stringData, file.dirEntry->d_name, FNM_PATHNAME)
    
    if (condition->params.caseSensitivity == INSENSITIVE)
    {
        free(fileName);
    }
    
    return (returnValue);
}



void tolowerarray(char * data)
{
    int i;
    for(i = 0; data[i] != '\0'; i++)
    {
        data[i] = tolower(data[i]);
    }
}