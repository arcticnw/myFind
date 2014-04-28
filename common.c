#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <string.h>

#include "common.h"


void 
string_to_lower(char *data)
{
    int i;
    for(i = 0; data[i] != '\0'; i++)
    {
        data[i] = tolower(data[i]);
    }
}


char * 
copy_string(char *source)
{
	char * target;
	
	if (!source) 
	{
		return (NULL);
	}
	
	target = malloc(sizeof(char) * (strlen(source) + 1));
	
	if (!target)
	{
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}
	
	strcpy(target, source);
	return (target);
}