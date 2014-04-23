#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "checker.h"
#include "parser.h"

void dump(condition_t *);

void dumpData(data_t * data, contents_t content)
{
    if (!data)
    {
	printf("#");
	return;
    }
    
    if (content == STRING)
    {
	printf("S(%s)", data->stringData);
    }
    else if (content == INT)
    {
	printf("I(%d)", data->intData);
    }
    else if (content == CONDITION)
    {
	printf("C(");
	dump(data->conditionData);
	printf(")");
    }
    else if (content == NONE)
    {
	printf("E");
    }
    else
    {
	printf("?");
    }
}
void dump(condition_t * c)
{
    if (!c)
    {
	printf("#");
	return;
    }
    printf("[");
    dumpData(c->data1, c->data1content);
    printf("; ");
    dumpData(c->data2, c->data2content);
    printf("]");
}

int main(int argc, char ** argv)
{
    condition_t * c;
    
	printf("hello\n");
	
	c = parseArguments(argc, argv);
	dump(c);
	disposeCondition(c);
	
	printf("bye\n");
	return 0;
}
