#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <assert.h>

#include "header.h"
#include "action.h"

void doPrint(action_t * action, file_t file)
{
	printf("%s\n", file->localPath);
}

void doExecute(action_t * action, file_t file)
{

}