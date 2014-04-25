#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <assert.h>

#include "header.h"
#include "parser.h"
#include "checker.h"
#include "action.h"

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
	
	if (!argsBundle.action)
	{
		appendAction(&argsBundle, createDefaultAction());
	}
	
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

action_t * createDefaultAction()
{
	action_t * action;
	
	action = createAction();
	action->doProcess = doPrint;
	
	return (action);
}

void disposeArgsBundle(argsBundle_t * argsBundle)
{
	if (argsBundle->condition)
	{
		disposeCondition(argsBundle->condition);
		free(argsBundle->condition);
	}
	if (argsBundle->action)
	{
		disposeAction(argsBundle->action);
		free(argsBundle->action);
	}
	
	initializeArgsBundle(argsBundle);
}

void disposeCondition(condition_t * condition)
{
	condition->doCheck = NULL;
	
	if (condition->data1content == STRING)
	{
		free(condition->data1.stringData);
	}
	else if (condition->data1content == CONDITION)
	{
		disposeCondition(condition->data1.conditionData);
		free(condition->data1.conditionData);
	}	
	condition->data1content = NONE;

	if (condition->data2content == STRING)
	{
		free(condition->data2.stringData);
	}
	else if (condition->data2content == CONDITION)
	{
		disposeCondition(condition->data2.conditionData);
		free(condition->data2.conditionData);
	}
	condition->data2content = NONE;
}

void disposeAction(action_t * action)
{
	int i;
	
	action->doProcess = NULL;
	if (action->params)
	{
		for(i = 0; i < action->paramsCount; i++)
		{
			free(action->params[i]);
		}
		free(action->params);
	}
	if (action->next)
	{
		disposeAction(action->next);
		free(action->next);
	}
}


void dumpData(data_t data, contents_t content)
{
	if (content == STRING)
	{
		printf("%s", data.stringData);
	}
	else if (content == INT)
	{
		printf("%d", data.intData);
	}
	else if (content == CONDITION)
	{
		dumpCondition(data.conditionData);
	}
	else if (content == NONE)
	{
		printf("-");
	}
	else
	{
		printf("?");
	}
}

void dumpCondition(condition_t * c)
{
	if (!c)
	{
		printf("#NULL#");
		return;
	}
	
	if (c->doCheck == checkOr || c->doCheck == checkAnd)
	{
		printf("[");
	}
	else if (c->doCheck == checkNot)
	{
		printf("!(");
	}
	else if (c->doCheck == checkTrue)
	{
		printf("<True> /*")
	}
	else if (c->doCheck == checkFalse)
	{
		printf("<False> /*")
	}
	else
	{
		printf("<Fct> (");
	}
	
	dumpData(c->data1, c->data1content);
	
	if (c->doCheck == checkOr)
	{
		printf("] or [");
	}
	else if (c->doCheck == checkAnd)
	{
		printf("] and [");
	}
	else if (c->doCheck == checkNot)
	{
		printf(" /*");
	}
	else
	{
		printf(", ");
	}
	
	dumpData(c->data2, c->data2content);
	
	if (c->doCheck == checkOr || c->doCheck == checkAnd)
	{
		printf("]");
	}
	else if (c->doCheck == checkTrue)
	{
		printf("*/")
	}
	else if (c->doCheck == checkFalse)
	{
		printf("*/")
	}
	else if (c->doCheck == checkNot)
	{
		printf("*/)");
	}
	else
	{
		printf(")");
	}	
}


condition_t * createConditionNode()
{
	condition_t * condition = (condition_t *)malloc(sizeof(condition_t));
	if (!condition)
	{
		err(1, "createConditionNode - malloc(condition)\n");
	}
	
	condition->doCheck = checkTrue;
	condition->data1.intData = 0;
	condition->data1content = NONE;
	condition->data2.intData = 0;
	condition->data2content = NONE;
	
	return (condition);
}

data_t createStringData(const char * originalData)
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
	
	return (data);
}

data_t createIntData(const char * originalData, signed char comparison)
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
		comparison = -1;
	}
	else if (originalData[0] == '-')
	{
		parseData++;
		comparison = +1;
	}
	else
	{
		comparison = 0;
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


condition_t * mergeConditionNodes(condition_t * condition1, condition_t * condition2, checker_t doChecker)
{
	condition_t * condition = createConditionNode();
	condition->doCheck = doChecker;
	condition->data1 = createConditionData(condition1);
	condition->data1content = CONDITION;
	condition->data2 = createConditionData(condition2);
	condition->data2content = CONDITION;
	
	return (condition);
}


condition_t * makeStringCondition(checker_t checker)
{
	condition_t * condition;

	if (position >= argc) 
	{ 
		err(2, "parseStringCondition - argument error, argument missing\n"); 
	}

	condition = createConditionNode();
	condition->doCheck = checker;
	
	condition->data1 = createStringData(argv[position]);
	condition->data1content = STRING;
	position++;

	return (condition);
}

condition_t * makeStringStringCondition(checker_t checker)
{
	condition_t * condition;

	if (position >= argc) 
	{ 
		err(2, "parseStringStringCondition - argument error, argument missing\n"); 
	}

	condition = createConditionNode();
	condition->doCheck = checker;
	
	condition->data1 = createStringData(argv[position]);
	condition->data1content = STRING;
	position++;
	
	condition->data2 = createStringData(argv[position]);
	condition->data2content = STRING;
	position++;

	return (condition);
}

condition_t * makeIntCondition(checker_t checker)
{
	condition_t * condition;

	if (position >= argc) 
	{ 
		err(2, "parseIntCondition - argument error, argument missing\n"); 
	}
	
	condition = createConditionNode();
	condition->doCheck = checker;
	
	condition->data1 = createIntData(argv[position], &(condition->params.compareMethod));
	condition->data1content = INT;
	
	position++;

	return (condition); 
}

action_t * createAction()
{
	action_t * action;
	action = (action_t*)malloc(sizeof(action_t));
	if (!action)
	{
		err(1, "createAction - malloc(action)\n");
	}
	action->doProcess = NULL;
	action->paramsCount = -1;
	action->next = NULL;
	action->params = NULL;
	return (action);
}
void appendAction(argsBundle_t * argsBundle, action_t * action)
{
	action_t * cAction;
	assert(argsBundle);
	
	/* if there's no action yet, append directly */
	if (!argsBundle->action)
	{
		argsBundle->action = action;
		return;
	}
	
	/* find the last action */
	for(cAction = argsBundle->action; cAction->next; cAction = cAction->next) ;
	
	cAction->next = action;
}

condition_t * tryParseCondition(char * cArg, argsBundle_t * argsBundle)
{
	condition_t * condition = NULL;
	
	if (!strcmp(cArg, "name"))
	{
		condition = makeStringCondition(checkName);
		condition->params.isCaseSensitive = 1;		
	}
	else if (!strcmp(cArg, "iname"))
	{
		condition = makeStringCondition(checkName);
		tolowerarray(condition->data1.stringData);
		condition->params.isCaseSensitive = 0;
	}
	
	return (condition);
}
int tryParseAction(char * cArg, argsBundle_t * argsBundle)
{
	action_t * action;
	int i;
	int startPosition;
	int endPosition;
	
	if (!strcmp(cArg, "exec"))
	{
		action = createAction();
		action->doProcess = doExecute;
		
		startPosition = position;
		for (counter = 0; strcmp(cArg, ";"); counter++)
		{
			while (position >= argc) 
			{ 
				err(2, "tryParseAction - argument error, ';' missing\n");
			}
			cArg = argv[position];
			position++;
		}
		/* the first cycle is comparing 'exec' to ';' => don't count that' */
		counter--;
		action->paramsCount = counter;
		
		if (counter)
		{
			endPosition = position;
			position = startPosition;
			
			action->params = (char **) malloc (sizeof(char*) * counter);
			if (!action->params)
			{
				err(1, "tryParseAction - malloc(params)\n");
			}
			for(i = 0; i < counter; i++)
			{
				cArg = argv[position];
				position++;
				
				action->params[i] = (char *)malloc(sizeof(char) * (strlen(cArg) + 1));
				if (!action->params[i])
				{
					err (1, "tryParseAction - malloc (params[i])\n");
				}
				strcpy(action->params[i], cArg);
			}
			
			position = endPosition;
		}
		
		appendAction(argsBundle, action);
	}
	else if (!strcmp(cArg, "print"))
	{
		action = createAction();
		action->doProcess = doPrint;
		appendAction(argsBundle, action);
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
	else if (!strcmp(cArg, "-noignore-hidden"))
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
		else if (!strcmp(cArg, "!") || !(strcmp(cArg, "not")))
		{
			condition = createConditionNode();
			condition->doCheck = checkNot;
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
			/* do nothing */
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