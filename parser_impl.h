#ifndef _PARSER_IMPL_H_
#define	_PARSER_IMPL_H_

#include "common.h"


/* this function initializes args_bundle_t and sets default values */
static args_bundle_t *
initialize_args_bundle();


/* this function creates default action to be applies on found files if */
/* no other action is specified */
static action_t *
create_default_action();


/* this function deallocates condition tree */
static void
dispose_condition(condition_t *condition);


/* this function deallocates linked list of action */
static void
dispose_action(action_t *action);


/* safely converts provided string to long */
static long
convert_string_to_long(const char * str);


/* this function initializes a data-holding struct from provided condition */
static void
create_condition_data(condition_t *condition, data_t *result);


/* this function initializes a data-holding struct from provided literal */
static void
create_string_data(const char *original_data, data_t *result);


/* this function initializes a data-holding struct from provided integer and */
/* determines the method of comparison (+ / -) */
static void
create_int_data(char *original_data, char *comparison, data_t *result);


/* this function initializes a new condition using the two provided */
/* conditions as operands and provided function as binary operator */
static condition_t *
merge_condition_nodes(condition_t *condition1, condition_t *condition2,
    check_t checker);


/* this function initializes new empty condition */
static condition_t *
make_empty_condition(check_t checker);


/* this function initializes new condition using provided check-function and */
/* an argument from argument list at current position as a string */
static condition_t *
make_string_condition(check_t checker);


/* this function initializes new condition using provided check-function and */
/* an argument from argument list at current position as an integer */
static condition_t *
make_int_condition(check_t checker);


/* this function initializes new empty action structure */
static action_t *
create_action();


/* this function appends an action at the end of a linked list of actions */
/* if no such list exists in args_bundle_t, then the action is used as */
/* a head of the list */
static void
append_action(args_bundle_t *args_bundle, action_t *action);


/* this function retrieves file status */
static void
retrieve_file_stat(const char *file_name, args_bundle_t *args_bundle,
    struct stat *result);


/* this function tries to make and return a condition from provided literal, */
/* if no such condition is made, NULL is returned */
static condition_t *
try_parse_condition(args_bundle_t *args_bundle);


/* this function tries to make an action from provided literal which is then */
/* added to the args_bundle_t, if no such action is made, 0 is returned */
static int
try_parse_action(args_bundle_t *args_bundle);


/* this function tries to make an option from provided literal which is then */
/* used in the args_bundle_t, if no such option is made, 0 is returned */
static int
try_parse_option(args_bundle_t *args_bundle);


/* this function makes a node of a condition tree from the current position */
/* in the argument list */
static condition_t *
build_condition_node(args_bundle_t *args_bundle);


/* this function makes a condition tree from the argument list */
static condition_t *
build_condition_tree(args_bundle_t *args_bundle);


/* this function check if the argument position is within range */
static void
increment_current_argument(char * expected);

#endif