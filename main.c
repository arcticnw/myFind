#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "check.h"

int test(data_t *test1, data_t *test2)
{
	return test1->intData + test2->intData;
}

int main(int argc, char ** argv)
{
	printf("hello\n");
	
	
	
	condition_t nCond;
	nCond.data1 = (data_t*)malloc(sizeof(data_t));
	nCond.data1->intData = 4;
	nCond.data2 = (data_t*)malloc(sizeof(data_t));
	nCond.data2->intData = 6;
	nCond.process = test;
	
	printf("the result is %d\n", nCond.process(nCond.data1, nCond.data2));
	
	free(nCond.data1);
	free(nCond.data2);
	
	printf("bye\n");
	return 0;
}
