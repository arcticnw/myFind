#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>

#include "header.h"
#include "parser.h"
#include "checker.h"

int position;
int argc;
char ** argv;

condition_t *
parseArgumentsToCondition(int pargc, char ** pargv)
{
    argc = pargc;
    argv = pargv;
    position = 2;  /* position 0 = executable name; position 1 = path */
    condition_t * condition = parseArgumentsNextJunction(argc, argv);
    return (condition);
}

void 
disposeCondition(condition_t * condition)
{
    if (!condition) 
    {
        return;
    }
    
    condition->process = NULL;
    
    if (condition->data1content == STRING)
    {
        free(condition->data1.stringData);
    }
    else if (condition->data1content == CONDITION)
    {
        disposeCondition(condition->data1.conditionData);
    }    
    condition->data1content = NONE;

    if (condition->data2content == STRING)
    {
        free(condition->data2.stringData);
    }
    else if (condition->data2content == CONDITION)
    {
        disposeCondition(condition->data2.conditionData);
    }
    condition->data2content = NONE;
    
    free(condition);    
    return;
}



condition_t * 
createConditionNode()
{
    condition_t * condition = (condition_t *)malloc(sizeof(condition_t));
    if (!condition)
    {
        err(1, "createConditionNode - malloc(condition)");
    }
    
    condition->process = checkTrue;
    condition->data1.intData = 0;
    condition->data1content = NONE;
    condition->data2.intData = 0;
    condition->data2content = NONE;
    
    return (condition);
}

data_t 
createStringDataNode(char * originalData)
{
    int dataSize;
    data_t data;
    
    dataSize = strlen(originalData) + 1;
    
    data.stringData = (char *)malloc(sizeof(char) * dataSize);
    if (!data.stringData)
    {
        err (1, "createStringDataNode - malloc (stringData)");
    }
    strcpy(data.stringData, originalData);
    data.stringData[dataSize-1] = '\0';
    
    return (data);
}

data_t
createIntDataNode(char * originalData, comparison_t * comparison)
{
    char * parseData = originalData;
    int value;
    data_t data;
    
    if (strlen(originalData) == 0)
    { 
        err(2, "createIntDataNode - argument error, empty string"); 
    }
    
    if (!strcmp(originalData[0], "+"))
    {
        parseData++;
        if (comparison)
        {
            *comparison = GREATERTHAN;
        }
    }
    else if (!strcmp(originalData[0], "-"))
    {
        parseData++;
        if (comparison)
        {
            *comparison = SMALLERTHAN;
        }
    }
    else if (comparison)
    {
        *comparison = EQUALTO;
    }
    
    value = atoi(parseData);
    
    data.intData = value;
    
    return (data);
}

data_t 
createConditionDataNode(condition_t * condition)
{
    data.conditionData = condition;
    
    return (data);
}

condition_t * 
mergeConditionNodes(condition_t * condition1, condition_t * condition2, processer_t processer)
{
    condition_t * condition = createConditionNode();
    condition->process = processer;
    condition->data1 = createConditionDataNode(condition1);
    condition->data1content = CONDITION;
    condition->data2 = createConditionDataNode(condition2);
    condition->data2content = CONDITION;
    
    return (condition);
}

condition_t * 
parseStringCondition(processer_t processer)
{
    if (position >= argc) 
    { 
        err(2, "parseStringCondition - argument error, argument missing"); 
    }

    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createStringDataNode(argv[position]);
    condition->data1content = STRING;
    position++;

    return (condition);
}

condition_t * 
parseStringStringCondition(processer_t processer)
{
    if (position >= argc) 
    { 
        err(2, "parseStringStringCondition - argument error, argument missing"); 
    }

    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createStringDataNode(argv[position]);
    condition->data1content = STRING;
    position++;
    
    condition->data2 = createStringDataNode(argv[position]);
    condition->data2content = STRING;
    position++;

    return (condition);
}

condition_t *
parseIntCondition(processer_t processer)
{
    if (position >= argc) 
    { 
        err(2, "parseIntCondition - argument error, argument missing"); 
    }
    
    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createIntDataNode(argv[position], &(condition->params.comparison));
    condition->data1content = INT;
    
    position++;

    return (condition); 
}

condition_t * 
parseArgumentsNext()
{
    char * cArg;
    condition_t * condition = NULL;
    
    if (position >= argc) 
    { 
        err(2, "parseArgumentsNext - argument error, argument missing"); 
    }
    
    cArg = argv[position];
    position++;
    
    if (!strcmp(cArg, "("))
    {
        condition = parseArgumentsNextJunction();
    }
    else if (!strcmp(cArg, "!"))
    {
        condition = createConditionNode();
        condition->process = checkNot;
        condition->data1 = createConditionDataNode(parseArgumentsNext());
        condition->data1content = CONDITION;
    }
    else if (!strcmp(cArg, "name"))
    {
        condition = parseStringCondition(checkName);
        condition->params.caseSensitivity = SENSITIVE;
    }
    else if (!strcmp(cArg, "iname"))
    {
        condition = parseStringCondition(checkName);
        condition->params.caseSensitivity = INSENSITIVE;
    }
    else if (!strcmp(cArg, ")"))
    {
        err(2, "parseArgumentsNext - argument error, misguided )");
    }
    else
    {
        err(2, "parseArgumentsNext - unknown token, %s", cArg);
    }
    
    return (condition);
}


condition_t * 
parseArgumentsNextJunction(
{
    char * cArg = NULL;
    condition_t * condition = NULL;
    condition_t * conditionBuf = NULL;
    condition_t * conditionTemp = NULL;
    
    /* Fetch the first condition explicitly */
    condition = parseArgumentsNext();
        
    while (position < argc)
    {
        cArg = argv[position];
        position++;
        
        if (!strcmp(cArg, ")"))
        {
            break;
        }
        else if (!strcmp(cArg, "or"))
        {
            conditionTemp = parseArgumentsNext();
            
            if (!conditionBuf)
            {
                /* buffer is empty => fill the buffer */
                conditionBuf = conditionTemp;
            }
            else
	    {
                /* buffer is not empty => merge the buffer with main condition
                     via 'OR' operation and refill the buffer */
                condition = mergeConditionNodes(condition, conditionBuf, checkOr);
                conditionBuf = conditionTemp;
            }

            conditionTemp = NULL;
        }
        else
        {
            /* implicit 'AND' operation between arguments */
            if (strcmp(cArg, "and"))
            {
                /* current token isn't 'AND' => condition consumed => undo */
                position--;
            }
            conditionTemp = parseArgumentsNext();
            
            if (!conditionBuf)
            {
                /* buffer is empty => operation 'AND' is with left node */
                condition = mergeConditionNodes(condition, conditionTemp, checkAnd);
            }
            else
            {
                /* buffer is not empty => operation 'AND' is with the right node */
                conditionBuf = mergeConditionNodes(conditionBuf, conditionTemp, checkAnd);
            }
            
            conditionTemp = NULL;
        }
    }
    
    if (conditionBuf) 
    {
        /* buffer isn't empty => merge buffer with main condition via 'OR' operation */
        condition = mergeConditionNodes(condition, conditionBuf, checkOr);
        conditionBuf = NULL;
    }
    
    return (condition);
}