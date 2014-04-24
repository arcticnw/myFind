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
        printf("%s", data.stringData);
    }
    else if (content == INT)
    {
        printf("%d", data.intData);
    }
    else if (content == CONDITION)
    {
        /*printf("");*/
        dump(data.conditionData);
        /*printf("");*/
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
void dump(condition_t * c)
{
    if (!c)
    {
        printf("#NULL#");
        return;
    }
    if (c->process == checkOr || c->process == checkAnd)
    {
	printf("[");
    }
    else if (c->process == checkNot)
    {
	printf("!(");
    }
    else
    {
	printf("<F> (");
    }    
    dumpData(c->data1, c->data1content);
    if (c->process == checkOr)
    {
	printf("] or [");
    }
    else if (c->process == checkAnd)
    {
        printf("] and [");
    }
    else if (c->process == checkNot)
    {
	printf(") /*");
    }
    else
    {
	printf(", ");
    }
    dumpData(c->data2, c->data2content);
    if (c->process == checkOr || c->process == checkAnd)
    {
	printf("]");
    }
    else if (c->process == checkNot)
    {
	printf("*/");
    }
    else
    {
	printf(")");
    }    
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
    argsBundle_t args;
    
    printf("hello\n");
    
    if (argc < 2)
    {
        usage(argv[0]);
        return 0;
    }
    
    printf("parse\n");
    
    args = parseArguments(argc, argv);
    
    printf("condition: ");
    dump(args.condition);
    printf("\n"); 
    
    
    printf("crawl\n");
    crawl(args);
    
    printf("dispose\n");
    disposeArgsBundle(&args);
    
    printf("bye\n");
    
    return 0;
}
