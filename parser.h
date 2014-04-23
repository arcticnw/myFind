#ifndef _PARSER_H_
#define _PARSER_H_

#include "header.h"

condition_t *parseArgumentsToCondition(int argc, char ** argv);
void disposeCondition(condition_t * condition);

condition_t * createConditionNode();
data_t * createConditionDataNode(condition_t * condition);
data_t * createStringDataNode(char * originalData);
condition_t * mergeConditions(condition_t * condition1, condition_t * condition2, processer_t processer);

condition_t * parseArgumentsNext(int argc, char ** argv);
condition_t * parseArgumentsNextJunction(int argc, char ** argv);

#endif