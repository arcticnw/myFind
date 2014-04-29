#ifndef _PARSER_H_
#define	_PARSER_H_

#include "common.h"

#define	ARG_ERR_MSG "Argument parsing error at current_position %d: "
#define	ARG1_ERR_MSG ARG_ERR_MSG "%s"
#define	ARG2_ERR_MSG ARG_ERR_MSG "%s: %s"
#define	ARG2_FILE_ERR_MSG ARG_ERR_MSG "Unable to access file %s: %s"


/* this function sets defaults, checks and parses string arguments and        */
/*     returns them in the args_bundle_t structure                            */
args_bundle_t *
parse_arguments(int argc, char **argv);


/* this function initializes args_bundle_t and sets default values            */
args_bundle_t *
initialize_args_bundle();


/* this function creates default action to be applies on found files if       */
/*     no other action is specified                                           */
action_t *
create_default_action();


/* this function deallocates args_bundle_t and its data                       */
void 
dispose_args_bundle(args_bundle_t *args_bundle);


/* this function deallocates condition tree                                   */
void 
dispose_condition(condition_t *condition);


/* this function deallocates linked list of action                            */
void 
dispose_action(action_t *action);


/* this function initializes a data-holding struct from provided condition    */
data_t 
create_condition_data(condition_t *condition);


/* this function initializes a data-holding struct from provided literal      */
data_t 
create_string_data(const char *original_data);


/* this function initializes a data-holding struct from provided integer and  */
/*     determines the method of comparison (+ / -)                            */
data_t 
create_int_data(char *original_data, char *comparison);


/* this function initializes a new condition using the two provided           */
/*     conditions as operands and provided function as binary operator        */
condition_t *
merge_condition_nodes(condition_t *condition1, condition_t *condition2, 
    check_t checker);
	

/* this function initializes new empty condition                              */
condition_t *
make_empty_condition();


/* this function initializes new condition using provided check-function and  */
/*     an argument from argument list at current position as a string         */
condition_t *
make_string_condition(check_t checker);


/* this function initializes new condition using provided check-function and  */
/*     two arguments from argument list at current position as strings        */
condition_t *
make_string_string_condition(check_t checker);


/* this function initializes new condition using provided check-function and  */
/*     an argument from argument list at current position as an integer       */
condition_t *
make_int_condition(check_t checker);


/* this function initializes new empty action structure                       */
action_t *
create_action();


/* this function appends an action at the end of a linked list of actions     */
/*     if no such list exists in args_bundle_t, then the action is used as    */
/**    a head of the list                                                     */
void 
append_action(args_bundle_t *args_bundle, action_t *action);


/**/
struct stat
retrieve_file_stat(const char *file_name, args_bundle_t *args_bundle);


/* this function tries to make and return a condition from provided literal,  */
/*     if no such condition is made, NULL is returned                         */
condition_t *
try_parse_condition(char *current_argument, args_bundle_t *args_bundle);


/* this function tries to make an action from provided literal which is then  */
/*     added to the args_bundle_t, if no such action is made, 0 is returned   */
int 
try_parse_action(char *current_argument, args_bundle_t *args_bundle);


/* this function tries to make an option from provided literal which is then  */
/*     used in the args_bundle_t, if no such option is made, 0 is returned    */
int 
try_parse_option(char *current_argument, args_bundle_t *args_bundle);


/* this function makes a node of a condition tree from the current position   */
/*     in the argument list                                                   */
condition_t *
build_condition_node(args_bundle_t *args_bundle);


/* this function makes a condition tree from the argument list                */
condition_t *
build_condition_tree(args_bundle_t *args_bundle);


/* this function check if the argument position is within range               */
void
argument_range_check(char * expected);

#endif