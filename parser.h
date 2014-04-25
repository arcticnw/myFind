#ifndef _PARSER_H_
#define _PARSER_H_

#include "header.h"

argsBundle_t parseArguments(int argc, char ** argv);
void initializeArgsBundle(argsBundle_t * argsBundle);
action_t * createDefaultAction();

void disposeArgsBundle(argsBundle_t * argsBundle);
void disposeCondition(condition_t * condition);
void disposeAction(action_t * action);

condition_t * createConditionNode();
data_t createConditionData(condition_t * condition);
data_t createStringData(const char * originalData);
data_t createIntData(char * originalData, signed char * comparison);

condition_t * mergeConditionNodes(condition_t * condition1, condition_t * condition2, checker_t doChecker);

condition_t * makeStringCondition(checker_t checker);
condition_t * makeStringStringCondition(checker_t checker);
condition_t * makeIntCondition(checker_t checker);

action_t * createAction();
void appendAction(argsBundle_t * argsBundle, action_t * action);

condition_t * tryParseCondition(char * cArg, argsBundle_t * argsBundle);
int tryParseAction(char * cArg, argsBundle_t * argsBundle);
int tryParseOption(char * cArg, argsBundle_t * argsBundle);

condition_t * buildConditionNode(argsBundle_t * argsBundle);
condition_t * buildConditionTree(argsBundle_t * argsBundle);

#endif