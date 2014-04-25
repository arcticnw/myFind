#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "parser.h"
#include "crawler.h"

void usage(char * name)
{
    printf("Usage: %s path [options] [expression [expression [..]]]\n"
	    "\n"
		"where [options] are\n"
	    "-follow\n"
		"-nofollow\n"
		"-ignore-hidden\n"
		"-noignore-hidden\n"
		"\n"
		"where [expression] is\n"
		"name filename\n"
		"iname filename\n"
		"\n"
		"and\n"
		"or\n"
		"(\n"
		")\n"
		"not\n"
		"!\n"
		"\n"
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
    dumpCondition(args.condition);
    printf("\n"); 
    
    
    printf("crawl\n");
    crawl(args);
    
    printf("dispose\n");
    disposeArgsBundle(&args);
    
    printf("bye\n");
    
    return 0;
}
