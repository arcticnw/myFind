#include <err.h>

#include "header.h"
#include "parser.h"
#include "checker.h"

int checkTrue(data_t *data1, data_t *data2, file_t *file)
{
    return 1;
}
int checkFalse(data_t *data1, data_t *data2, file_t *file)
{
    return 0;
}

int checkAnd(data_t *data1, data_t *data2, file_t *file)
{
    int leftSide = 0;
    int rightSide = 0;
    
    if (!data1) 
    {
        err(3, "checkAnd - unexpected logic - left side is NULL"); 
    }
    if (!data1->conditionData)
    {
        err(3, "checkAnd - unexpected logic - left side isn't condition");
    }
    leftSide = data1->conditionData->process(
      data1->conditionData->data1, 
      data1->conditionData->data2, file);
      
    if (!data2) 
    {
        err(3, "checkAnd - unexpected logic - right side is NULL"); 
    }
    if (!data2->conditionData)
    {
        err(3, "checkAnd - unexpected logic - right side isn't condition");
    }
    rightSide = data2->conditionData->process(
      data2->conditionData->data1, 
      data2->conditionData->data2, file);
      
    return (leftSide && rightSide);
}
int checkOr(data_t *data1, data_t *data2, file_t *file)
{
    int leftSide = 0;
    int rightSide = 1;

    if (!data1) 
    {
        err(3, "checkAnd - unexpected logic - left side is NULL"); 
    }
    if (!data1->conditionData)
    {
        err(3, "checkAnd - unexpected logic - left side isn't condition");
    }
    leftSide = data1->conditionData->process(
      data1->conditionData->data1, 
      data1->conditionData->data2, file);
      
    if (data2) 
    {
        if (data2->conditionData)
        {
            rightSide = data2->conditionData->process(
              data2->conditionData->data1, 
              data2->conditionData->data2, file);
        }
    }
    return (leftSide || rightSide);
}

int checkName(data_t *data1, data_t *data2, file_t *file)
{
    return (1);
}