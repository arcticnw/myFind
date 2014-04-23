#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "checker.h"
#include "parser.h"

void dump(condition_t * c)
{
    
}

int main(int argc, char ** argv)
{
    condition_t * c;
    
	printf("hello\n");
	
	c = parseArguments(argc, argv);
    disposeCondition(c);
	
	printf("bye\n");
	return 0;
}
