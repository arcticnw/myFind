#ifndef _PARSER_H_
#define _PARSER_H_

#include "header.h"

argsBundle_t parseArguments(int argc, char ** argv);
void initializeArgsBundle(argsBundle_t * argsBundle);
void disposeArgsBundle(argsBundle_t argsBundle);
void disposeAction(action_t * action);
void disposeCondition(condition_t * condition);

condition_t * createConditionNode();
data_t createConditionData(condition_t * condition);
data_t createStringData(char * originalData);
data_t createIntData(char * originalData, comparison_t * comparison);

condition_t * mergeConditionNodes(condition_t * condition1, condition_t * condition2, processer_t processer);

condition_t * makeStringCondition(processer_t processer);
condition_t * makeStringStringCondition(processer_t processer);
condition_t * makeIntCondition(processer_t processer);

condition_t * tryParseCondition(char * cArg, argsBundle_t * argsBundle);
int tryParseAction(char * cArg, argsBundle_t * argsBundle);
int tryParseOption(char * cArg, argsBundle_t * argsBundle);

condition_t * buildConditionNode(argsBundle_t * argsBundle);
condition_t * buildConditionTree(argsBundle_t * argsBundle);

#endif