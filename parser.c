#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <assert.h>

#include "header.h"
#include "parser.h"
#include "checker.h"

int position;
int argc;
char ** argv;

argsBundle_t parseArguments(int pargc, char ** pargv)
{
    argsBundle_t argsBundle;

    argc = pargc;
    argv = pargv;
    
    initializeArgsBundle(&argsBundle);
    
    argsBundle.path = argv[1];
    
    position = 2;
    argsBundle.condition = buildConditionTree(&argsBundle);
    
    argc = 0;
    argv = NULL;
    
    return (argsBundle);
}

void initializeArgsBundle(argsBundle_t * argsBundle)
{
    argsBundle->followLinks = 0;
    argsBundle->ignoreHidden = 1;
    argsBundle->path = NULL;
    argsBundle->condition = NULL;
    argsBundle->action = NULL;
}

void disposeArgsBundle(argsBundle_t * argsBundle)
{
    disposeCondition(argsBundle->condition);
    disposeAction(argsBundle->action);
    
    initializeArgsBundle(argsBundle);
}

void disposeCondition(condition_t * condition)
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

void disposeAction(action_t * action)
{
    if (!action)
    {
        return;
    }
    /* TODO */
    free(action);
}


condition_t * createConditionNode()
{
    condition_t * condition = (condition_t *)malloc(sizeof(condition_t));
    if (!condition)
    {
        err(1, "createConditionNode - malloc(condition)\n");
    }
    
    condition->process = checkTrue;
    condition->data1.intData = 0;
    condition->data1content = NONE;
    condition->data2.intData = 0;
    condition->data2content = NONE;
    
    return (condition);
}

data_t createStringData(char * originalData)
{
    int dataSize;
    data_t data;
    
    dataSize = strlen(originalData) + 1;
    
    data.stringData = (char *)malloc(sizeof(char) * dataSize);
    if (!data.stringData)
    {
        err (1, "createStringData - malloc (stringData)\n");
    }
    strcpy(data.stringData, originalData);
    data.stringData[dataSize-1] = '\0';
    
    return (data);
}

data_t createIntData(char * originalData, comparison_t * comparison)
{
    char * parseData = originalData;
    int value;
    data_t data;
    
    if (strlen(originalData) == 0)
    { 
        err(2, "createIntData - argument error, empty string\n"); 
    }
    
    if (originalData[0] == '+')
    {
        parseData++;
        if (comparison)
        {
            *comparison = GREATERTHAN;
        }
    }
    else if (originalData[0] == '-')
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

data_t createConditionData(condition_t * condition)
{
    data_t data;

    data.conditionData = condition;
    
    return (data);
}


condition_t * mergeConditionNodes(condition_t * condition1, condition_t * condition2, processer_t processer)
{
    condition_t * condition = createConditionNode();
    condition->process = processer;
    condition->data1 = createConditionData(condition1);
    condition->data1content = CONDITION;
    condition->data2 = createConditionData(condition2);
    condition->data2content = CONDITION;
    
    return (condition);
}


condition_t * makeStringCondition(processer_t processer)
{
    condition_t * condition;

    if (position >= argc) 
    { 
        err(2, "parseStringCondition - argument error, argument missing\n"); 
    }

    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createStringData(argv[position]);
    condition->data1content = STRING;
    position++;

    return (condition);
}

condition_t * makeStringStringCondition(processer_t processer)
{
    condition_t * condition;

    if (position >= argc) 
    { 
        err(2, "parseStringStringCondition - argument error, argument missing\n"); 
    }

    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createStringData(argv[position]);
    condition->data1content = STRING;
    position++;
    
    condition->data2 = createStringData(argv[position]);
    condition->data2content = STRING;
    position++;

    return (condition);
}

condition_t * makeIntCondition(processer_t processer)
{
    condition_t * condition;

    if (position >= argc) 
    { 
        err(2, "parseIntCondition - argument error, argument missing\n"); 
    }
    
    condition = createConditionNode();
    condition->process = processer;
    
    condition->data1 = createIntData(argv[position], &(condition->params.comparison));
    condition->data1content = INT;
    
    position++;

    return (condition); 
}

condition_t * tryParseCondition(char * cArg, argsBundle_t * argsBundle)
{
    condition_t * condition = NULL;
    
    if (!strcmp(cArg, "name"))
    {
        condition = makeStringCondition(checkName);
        condition->params.caseSensitivity = SENSITIVE;        
    }
    else if (!strcmp(cArg, "iname"))
    {
        condition = makeStringCondition(checkName);
        tolowerarray(condition->data1.stringData);
        condition->params.caseSensitivity = INSENSITIVE;
    }
    
    return (condition);
}
int tryParseAction(char * cArg, argsBundle_t * argsBundle)
{
    if (0)
    {
    
    }
    else
    {
        return (0);
    }
    return (1);
}
int tryParseOption(char * cArg, argsBundle_t * argsBundle)
{
    if (!strcmp(cArg, "-follow"))
    {
        argsBundle->followLinks = 1;
    }
    else if (!strcmp(cArg, "-nofollow"))
    {
        argsBundle->followLinks = 0;
    }
    else if (!strcmp(cArg, "-ignore-hidden"))
    {
	argsBundle->ignoreHidden = 1;
    }
    else if (!strcmp(cArg, "-no-ignore-hidden"))
    {
	argsBundle->ignoreHidden = 0;
    }
    else
    {
        return (0);
    }
    return (1);
}

condition_t * buildConditionNode(argsBundle_t * argsBundle)
{
    char * cArg;
    condition_t * condition = NULL;
    
    while (position < argc)
    {
        cArg = argv[position];
        position++;
        
        if (!strcmp(cArg, "("))
        {
            condition = buildConditionTree(argsBundle);
            break;
        }
        else if (!strcmp(cArg, "!"))
        {
            condition = createConditionNode();
            condition->process = checkNot;
            condition->data1 = createConditionData(buildConditionNode(argsBundle));
            condition->data1content = CONDITION;
            break;
        }
        else if ((condition = tryParseCondition(cArg, argsBundle)))
        {
            break;
        }
        else if (tryParseAction(cArg, argsBundle))
        {
            /*  do nothing */
        }
        else if (tryParseOption(cArg, argsBundle))
        {
            /* do nothing */
        }
        else if (!strcmp(cArg, ")"))
        {
            fprintf(stderr, "buildConditionNode - ')' unexpected at this position, argument #%d\n", position-1);
            break;
        }
        else
        {
            fprintf(stderr, "buildConditionNode - unknown token, %s\n", cArg);
        }
    }
    return (condition);
}

condition_t * buildConditionTree(argsBundle_t * argsBundle)
{
    char * cArg = NULL;
    condition_t * condition = NULL;
    condition_t * conditionBuf = NULL;
    condition_t * conditionTemp = NULL;
    
    /* Fetch the first condition explicitly */
    condition = buildConditionNode(argsBundle);
   
    if (!condition)
    {
        return (NULL);
    }
    
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
            conditionTemp = buildConditionNode(argsBundle);
            if (!conditionTemp)
            {
                assert(position >= argc);
                fprintf(stderr, "buildConditionTree - binary operator 'or' used without second operand\n");
                break;
            }
            
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
            conditionTemp = buildConditionNode(argsBundle);
            
            if (!conditionTemp)
            {
                assert(position >= argc);
                /* fprintf(stderr, "buildConditionTree - binary operator 'and' used without second operand\n"); */
                break;
            }
            
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