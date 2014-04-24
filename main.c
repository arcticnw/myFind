#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "checker.h"
#include "parser.h"
#include "crawler.h"

void dump(condition_t *);

void dumpData(data_t data, contents_t content)
{
    if (content == STRING)
    {
        printf("S(%s)", data.stringData);
    }
    else if (content == INT)
    {
        printf("I(%d)", data.intData);
    }
    else if (content == CONDITION)
    {
        printf("C(");
        dump(data.conditionData);
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

void usage(char * name)
{
    printf("Usage: %s path [operand_expression ..]\n"
	    "\n"
	    "something something usage something something\n",
	    name);
}

int main(int argc, char ** argv)
{
    condition_t * cond;
    int retV;
    
    printf("hello\n");
    
    if (argc < 2)
    {
        usage(argv[0]);
        return 0;
    }
    
    cond = parseArgumentsToCondition(argc, argv);
    dump(cond);
    printf("\n");
    
    retV = crawl(argv[1], cond);
    
    disposeCondition(cond);
    
    printf("bye\n");
    
    return retV;
}
