#ifndef _PARSER_H_
#define _PARSER_H_

#include "header.h"

condition_t *
parseArgumentsToCondition(int argc, char ** argv);
void 
disposeCondition(condition_t * condition);

condition_t *
createConditionNode();
data_t
createConditionDataNode(condition_t * condition);
data_t
createStringDataNode(char * originalData);
data_t
createIntDataNode(char * originalData, comparison_t * comparison);

condition_t * 
mergeConditionNodes(condition_t * condition1, condition_t * condition2, processer_t processer);

condition_t * 
parseStringCondition(processer_t processer);
condition_t * 
parseStringStringCondition(processer_t processer);
condition_t * 
parseIntCondition(processer_t processer);

condition_t * 
parseArgumentsNext();
condition_t * 
parseArgumentsNextJunction();

#endif