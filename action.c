#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "action.h"

void do_print(action_t *action, file_t file)
{
	printf("%s\n", file.local_path);
}

void do_execute(action_t *action, file_t file)
{

}