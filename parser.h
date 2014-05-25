#ifndef _PARSER_H_
#define	_PARSER_H_

#include "common.h"


/* this function sets defaults, checks and parses string arguments and */
/* returns them in the args_bundle_t structure */
args_bundle_t *
parse_arguments(int argc, char **argv);


/* this function deallocates args_bundle_t and its data */
void
dispose_args_bundle(args_bundle_t *args_bundle);


#endif