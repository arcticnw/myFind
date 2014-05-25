#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

#define	MALLOC_ERR_MSG "Failed to allocate memory: %s"

void
string_to_lower(char *data) {
	int i;
	for (i = 0; data[i] != '\0'; i++) {
		data[i] = tolower(data[i]);
	}
}

char *
copy_string_part(const char *source, int start, int length) {
	char *target;
	size_t i;

	if (!source)  {
		return (NULL);
	}

	target = malloc(sizeof (char) * (length + 1));

	if (!target) {
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}

	for (i = start; i < (start+length) && source[i] != '\0'; i++) {
		target[i] = source[i];
	}

	for (; i < start+length+1; i++) {
		target[i] = '\0';
	}

	return (target);
}

char *
copy_string(const char *source) {
	char * target;

	if (!source)  {
		return (NULL);
	}

	target = malloc(sizeof (char) * (strlen(source) + 1));

	if (!target) {
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}

	strcpy(target, source);
	return (target);
}