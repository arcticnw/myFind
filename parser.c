#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>

#include "header.h"
#include "parser.h"
#include "checker.h"

int position;

condition_t *parseArguments(int argc, char **argv)
{
    position = 2;  /* position 0 = executable name; position 1 = path */
    /*printf("Parsing from position %d to %d\n", position, argc);*/
    return parseArgumentsNextJunction(argc, argv);
}

void disposeCondition(condition_t * condition)
{
    if (!condition) 
    {
        return;
    }
    
    condition->process = NULL;
    if (condition->data1)
    {
        if (condition->data1content == STRING)
        {
            free(condition->data1->stringData);
        }
        else if (condition->data1content == CONDITION)
        {
            disposeCondition(condition->data1->conditionData);
        }
        free(condition->data1);
        condition->data1 = NULL;
        condition->data1content = NONE;
    }
    if (condition->data2)
    {
        if (condition->data2content == STRING)
        {
            free(condition->data2->stringData);
        }
        else if (condition->data2content == CONDITION)
        {
            disposeCondition(condition->data2->conditionData);
        }
        free(condition->data2);
        condition->data2 = NULL;
        condition->data2content = NONE;
    }
    return;
}



condition_t * createConditionNode()
{
    condition_t * condition = (condition_t *)malloc(sizeof(condition_t));
    if (!condition)
    {
        return (NULL);
    }
    
    condition->process = checkTrue;
    condition->data1 = NULL;
    condition->data1content = NONE;
    condition->data2 = NULL;
    condition->data2content = NONE;
    
    printf("Creating empty condition\n");
    
    return (condition);
}

data_t * createStringDataNode(char * originalData)
{
    int dataSize;
    data_t * data;
    
    dataSize = strlen(originalData);
    
    data = (data_t *)malloc(sizeof(data_t));
    if (!data)
    {
        err (1, "createStringDataNode - malloc (data)");
    }
    
    data->stringData = (char *)malloc(sizeof(char) * dataSize);
    if (!data->stringData)
    {
        err (1, "createStringDataNode - malloc (stringData)");
    }
    strcpy(data->stringData, originalData);
    
    printf("Creating string data %s\n", originalData);
    
    return (data);
}

data_t * createConditionDataNode(condition_t * condition)
{
    data_t * data;
    
    data = (data_t *)malloc(sizeof(data_t));
    if (!data)
    {
        err (1, "createConditionDataNode - malloc (data)");
    }
    
    data->conditionData = condition;
    
    printf("Creating condition data\n");
    
    return (data);
}

condition_t * mergeConditions(condition_t * condition1, condition_t * condition2, processer_t processer)
{
    condition_t * condition = createConditionNode();
    /* assert(condition); / * already handled */
    condition->process = processer;
    condition->data1 = createConditionDataNode(condition1);
    condition->data1content = CONDITION;
    condition->data2 = createConditionDataNode(condition2);
    condition->data2content = CONDITION;
    
    printf("Merging conditions\n");
    
    return (condition);
}

condition_t * parseArgumentsNext(int argc, char **argv)
{
    char * cArg;
    condition_t * condition;
    
    printf("Entering PAN position: %d\n", position);
    
    for (; position < argc; position++)
    {
	cArg = argv[position];
        printf("PAN: %s\n", cArg);
        if (!strcmp(cArg, "("))
        {
            position++;
            return (parseArgumentsNextJunction(argc, argv));
        }
        else if (!strcmp(cArg, "name"))
        {
            position++;
            cArg = argv[position];

            condition = createConditionNode();
            condition->process = checkName;
            condition->data1 = createStringDataNode(cArg);
            condition->data1content = STRING;
            
            position++;
            
            return (condition);
        }
        else if (!strcmp(cArg, ")"))
        {
            err(2, "parseArgumentsNext - argument error, misguided )");
        }
    }
    printf("Leaving PAN\n");
    /*err(2, "parseArgumentsNext - argument error, unknown state");*/
    return (createConditionNode());
}


condition_t * parseArgumentsNextJunction(int argc, char **argv)
{
    char * cArg = NULL;
    condition_t * condition = NULL;
    condition_t * conditionBuf = NULL;
    condition_t * conditionTemp = NULL;
    
    printf("Entering PANJ position: %d\n", position);
    
    /* Fetch the first condition explicitly */
    condition = parseArgumentsNext(argc, argv);
    /* assert(condition); / * already handled */
        
    for (; position < argc; position++)
    {
        cArg = argv[position];
        printf("PANJ: %s\n", cArg);
        if (!strcmp(cArg, ")"))
        {
            position++;
            break;
        }
        else if (!strcmp(cArg, "or"))
        {
            position++;
            
            conditionTemp = parseArgumentsNext(argc, argv);
            /* assert(conditionTemp); / * already handled */
            
            if (!conditionBuf)
            {
                /* buffer is empty => fill the buffer */
                conditionBuf = conditionTemp;
            }
            else
	    {
                /* buffer is not empty => merge the buffer with main condition
                     via 'OR' operation and refill the buffer */
                condition = mergeConditions(condition, conditionBuf, checkOr);
                conditionBuf = conditionTemp;
            }

            conditionTemp = NULL;
        }
        else
        {
            /* implicit 'AND' operation between arguments */
            if (!strcmp(cArg, "and"))
            {
                position++;
            }
            conditionTemp = parseArgumentsNext(argc, argv);
            /* assert(conditionTemp); / * already handled */
            
            if (!conditionBuf)
            {
                /* buffer is empty => operation 'AND' is with left node */
                condition = mergeConditions(condition, conditionTemp, checkAnd);
            }
            else
            {
                /* buffer is not empty => operation 'AND' is with the right node */
                conditionBuf = mergeConditions(conditionBuf, conditionTemp, checkAnd);
            }
            
            conditionTemp = NULL;
        }
    }
    
    if (conditionBuf) 
    {
        /* buffer isn't empty => merge buffer with main condition via 'OR' operation */
        condition = mergeConditions(condition, conditionBuf, checkOr);
        conditionBuf = NULL;
    }
    
    printf("Leaving PANJ\n");
    return (condition);
}