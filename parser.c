#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"
#include "parser.h"
#include "checker.h"
#include "action.h"


int next_arg_index;
int argument_count;
char **argument_data;


args_bundle_t *
parse_arguments(int argc, char **argv)
{
	args_bundle_t *args_bundle;

	argument_count = argc;
	argument_data = argv;
	next_arg_index = 2;
	
	args_bundle = initialize_args_bundle();
	args_bundle->path = argument_data[1];
	args_bundle->condition = build_condition_tree(args_bundle);
	
	if (!args_bundle->action)
	{
		append_action(args_bundle, create_default_action());
	}
	
	argument_count = 0;
	argument_data = NULL;
	
	return (args_bundle);
}


args_bundle_t *
initialize_args_bundle()
{
	args_bundle_t *args_bundle;
	args_bundle = malloc(sizeof(args_bundle_t));
	if (!args_bundle)
	{
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}
	args_bundle->follow_links = 0;
	args_bundle->ignore_hidden = 1;
	time(&(args_bundle->time_now));
	args_bundle->path = NULL;
	args_bundle->condition = NULL;
	args_bundle->action = NULL;
	
	return (args_bundle);
}


action_t *
create_default_action()
{
	action_t *action;
	action = create_action();
	action->do_action = do_print;
	return (action);
}


void 
dispose_args_bundle(args_bundle_t *args_bundle)
{
	if (args_bundle->condition)
	{
		dispose_condition(args_bundle->condition);
	}
	if (args_bundle->action)
	{
		dispose_action(args_bundle->action);
	}
	
	free(args_bundle);
}


void 
dispose_condition(condition_t *condition)
{
	assert(condition);
	
	condition->do_check = NULL;
	
	if (condition->data1_content == STRING)
	{
		free(condition->data1.string_data);
	}
	else if (condition->data1_content == CONDITION)
	{
		dispose_condition(condition->data1.condition_data);
	}	
	condition->data1_content = NONE;

	if (condition->data2_content == STRING)
	{
		free(condition->data2.string_data);
	}
	else if (condition->data2_content == CONDITION)
	{
		dispose_condition(condition->data2.condition_data);
	}
	condition->data2_content = NONE;
	
	free(condition);
}


void 
dispose_action(action_t *action)
{
	int i;
	
	assert(action);
	
	action->do_action = NULL;
	if (action->params)
	{
		for(i = 0; i < action->param_count; i++)
		{
			free(action->params[i]);
		}
		free(action->params);
	}
	if (action->next)
	{
		dispose_action(action->next);
	}
	free(action);
}


data_t 
create_condition_data(condition_t *condition)
{
	data_t data;
	data.condition_data = condition;
	
	return (data);
}


data_t 
create_string_data(const char *original_data)
{
	data_t data;
	data.string_data = copy_string(original_data);
	
	return (data);
}


data_t 
create_int_data(char *original_data, char *comparison)
{
	char *parse_data = original_data;
	long value;
	data_t data;
	
	assert(original_data);
	assert(strlen(original_data));
	
	if (original_data[0] == '+')
	{
		parse_data++;
		*comparison = '+';
	}
	else if (original_data[0] == '-')
	{
		parse_data++;
		*comparison = '-';
	}
	else
	{
		*comparison = ' ';
	}
	
	value = atol(parse_data);
	data.long_data = value;
	
	return (data);
}


condition_t *
merge_condition_nodes(condition_t *condition1, condition_t *condition2, check_t checker)
{
	condition_t *condition = make_empty_condition();
	condition->do_check = checker;
	condition->data1 = create_condition_data(condition1);
	condition->data1_content = CONDITION;
	condition->data2 = create_condition_data(condition2);
	condition->data2_content = CONDITION;
	
	return (condition);
}


condition_t *
make_empty_condition()
{
	condition_t *condition = malloc(sizeof(condition_t));
	if (!condition)
	{
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}
	
	condition->do_check = check_true;
	condition->data1.long_data = 0;
	condition->data1_content = NONE;
	condition->data2.long_data = 0;
	condition->data2_content = NONE;
	
	return (condition);
}


condition_t *
make_string_condition(check_t checker)
{
	condition_t *condition;

	argument_range_check("String argument expected");

	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_string_data(argument_data[next_arg_index]);
	condition->data1_content = STRING;
	next_arg_index++;

	return (condition);
}


condition_t *
make_string_string_condition(check_t checker)
{
	condition_t *condition;

	argument_range_check("String argument expected");

	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_string_data(argument_data[next_arg_index]);
	condition->data1_content = STRING;
	next_arg_index++;
	
	condition->data2 = create_string_data(argument_data[next_arg_index]);
	condition->data2_content = STRING;
	next_arg_index++;

	return (condition);
}


condition_t *
make_int_condition(check_t checker)
{
	condition_t *condition;

	argument_range_check("Integer argument expected");
	
	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_int_data(argument_data[next_arg_index], &(condition->params.compare_method));
	condition->data1_content = LONG;
	
	next_arg_index++;

	return (condition); 
}


action_t *
create_action()
{
	action_t *action;
	action = malloc(sizeof(action_t));
	if (!action)
	{
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}
	action->do_action = NULL;
	action->param_count = -1;
	action->next = NULL;
	action->params = NULL;
	return (action);
}


void 
append_action(args_bundle_t *args_bundle, action_t *action)
{
	action_t *current_action;
	assert(args_bundle);
	
	/* if there's no action yet, append directly */
	if (!args_bundle->action)
	{
		args_bundle->action = action;
		return;
	}
	
	/* find the last action */
	current_action = args_bundle->action;
	while(current_action->next)
	{
		current_action = current_action->next;
	}
	
	/* append */
	current_action->next = action;
}


struct stat
retrieve_file_stat(const char *file_name, args_bundle_t *args_bundle)
{
	struct stat file_entry_stat;
	
	if (args_bundle->follow_links && 
		stat(file_name, &file_entry_stat))
	{
		errx(2, ARG2_FILE_ERR_MSG, next_arg_index--, file_name, strerror(errno));
	}
	if (!args_bundle->follow_links && 
		lstat(file_name, &file_entry_stat))
	{
		errx(2, ARG2_FILE_ERR_MSG, next_arg_index--, file_name, strerror(errno));
	}
	
	return (file_entry_stat);
}


condition_t *
try_parse_condition(char *current_argument, args_bundle_t *args_bundle)
{
	condition_t *condition = NULL;
	struct stat file_entry_stat;
	char * file_name;
	
	if (!strcmp(current_argument, "true"))
	{
		condition = make_empty_condition();
		condition->do_check = check_true;
	}
	else if (!strcmp(current_argument, "false"))
	{
		condition = make_empty_condition();
		condition->do_check = check_false;
	}
	else if (!strcmp(current_argument, "name"))
	{
		condition = make_string_condition(check_name);
		condition->params.is_case_sensitive = 1;		
	}
	else if (!strcmp(current_argument, "iname"))
	{
		condition = make_string_condition(check_name);
		string_to_lower(condition->data1.string_data);
		condition->params.is_case_sensitive = 0;
	}
	else if (!strcmp(current_argument, "amin"))
	{
		condition = make_int_condition(check_atime);
		condition->data2.long_data = 60; /* comparing by minutes */
		condition->data2_content = LONG;
	} 
	else if (!strcmp(current_argument, "atime"))
	{
		condition = make_int_condition(check_atime);
		condition->data2.long_data = 60*60*24; /* comparing by days */
		condition->data2_content = LONG;
	}
	else if (!strcmp(current_argument, "anewer"))
	{
		condition = make_empty_condition();
		condition->do_check = check_atime;
		condition->params.compare_method = '-';
		
		argument_range_check("String argument expected");

		file_name = copy_string(argument_data[next_arg_index]);
		next_arg_index++;

		file_entry_stat = retrieve_file_stat(file_name, args_bundle);

		condition->data1.long_data = args_bundle->time_now - file_entry_stat.st_atime;
		condition->data1_content = LONG;
		condition->data2.long_data = 1;
		condition->data2_content = LONG;
	}
	else if (!strcmp(current_argument, "cmin"))
	{
		condition = make_int_condition(check_ctime);
		condition->data2.long_data = 60; /* comparing by minutes */
		condition->data2_content = LONG;
	} 
	else if (!strcmp(current_argument, "ctime"))
	{
		condition = make_int_condition(check_ctime);
		condition->data2.long_data = 60*60*24; /* comparing by days */
		condition->data2_content = LONG;
	}
	else if (!strcmp(current_argument, "cnewer"))
	{
		condition = make_empty_condition();
		condition->do_check = check_ctime;
		condition->params.compare_method = '-';
		
		argument_range_check("String argument expected");

		file_name = copy_string(argument_data[next_arg_index]);
		next_arg_index++;

		file_entry_stat = retrieve_file_stat(file_name, args_bundle);

		condition->data1.long_data = args_bundle->time_now - file_entry_stat.st_atime;
		condition->data1_content = LONG;
		condition->data2.long_data = 1;
		condition->data2_content = LONG;
	}
	else if (!strcmp(current_argument, "mmin"))
	{
		condition = make_int_condition(check_mtime);
		condition->data2.long_data = 60; /* comparing by minutes */
		condition->data2_content = LONG;
	} 
	else if (!strcmp(current_argument, "mtime"))
	{
		condition = make_int_condition(check_mtime);
		condition->data2.long_data = 60*60*24; /* comparing by days */
		condition->data2_content = LONG;
	}
	else if (!strcmp(current_argument, "mnewer"))
	{
		condition = make_empty_condition();
		condition->do_check = check_mtime;
		condition->params.compare_method = '-';
		
		argument_range_check("String argument expected");

		file_name = copy_string(argument_data[next_arg_index]);
		next_arg_index++;

		file_entry_stat = retrieve_file_stat(file_name, args_bundle);

		condition->data1.long_data = args_bundle->time_now - file_entry_stat.st_atime;
		condition->data1_content = LONG;
		condition->data2.long_data = 1;
		condition->data2_content = LONG;
	}
	return (condition);
}


int 
try_parse_action(char *current_argument, args_bundle_t *args_bundle)
{
	action_t *action;
	int i;
	int start_position;
	int end_position;
	int exec_args_count;
	
	if (!strcmp(current_argument, "exec"))
	{ 
		/* create action */
		action = create_action();
		action->do_action = do_execute;
		
		/* backup argument position exec_args_count */
		start_position = next_arg_index;
				
		/* count the number of subsequent arguments until ';' */
		for (exec_args_count = 0; strcmp(current_argument, ";"); exec_args_count++)
		{
			argument_range_check("';' expected");

			current_argument = argument_data[next_arg_index];
			next_arg_index++;
		}
		/* the first cycle is comparing 'exec' to ';' => don't count that' */
		exec_args_count--;
		
		/* make sure there is something to run */
		if (!exec_args_count)
		{
			errx(1, ARG1_ERR_MSG, next_arg_index - 1, "String argument expected");
		}

		action->param_count = exec_args_count;		
		
		/* restore argument position exec_args_count */
		end_position = next_arg_index;
		next_arg_index = start_position;
		
		/* allocate parameters */
		action->params = malloc (sizeof(char*) * (exec_args_count + 1));
		if (!action->params)
		{
			errx(127, MALLOC_ERR_MSG, strerror(errno));
		}
		
		/* copy the literals */
		for(i = 0; i < exec_args_count; i++)
		{
			current_argument = argument_data[next_arg_index];
			next_arg_index++;
			
			action->params[i] = copy_string(current_argument);
		}
		action->params[exec_args_count] = NULL;
		append_action(args_bundle, action);
		
		next_arg_index = end_position;
	}
	else if (!strcmp(current_argument, "print"))
	{
		action = create_action();
		action->do_action = do_print;
		append_action(args_bundle, action);
	}
	else
	{
		/* no match found => not an action */
		return (0);
	}
	return (1);
}


int 
try_parse_option(char *current_argument, args_bundle_t *args_bundle)
{
	if (!strcmp(current_argument, "-follow"))
	{
		args_bundle->follow_links = 1;
	}
	else if (!strcmp(current_argument, "-no-follow"))
	{
		args_bundle->follow_links = 0;
	}
	else if (!strcmp(current_argument, "-ignore-hidden"))
	{
		args_bundle->ignore_hidden = 1;
	}
	else if (!strcmp(current_argument, "-no-ignore-hidden"))
	{
		args_bundle->ignore_hidden = 0;
	}
	else
	{
		/* no match found => not an option */
		return (0);
	}
	return (1);
}


condition_t *build_condition_node(args_bundle_t *args_bundle)
{
	char *current_argument;
	condition_t *condition = NULL;
	
	while (next_arg_index < argument_count)
	{
		current_argument = argument_data[next_arg_index];
		next_arg_index++;
		
		if (!strcmp(current_argument, "("))
		{
			/* start a new eval. tree in the ( ) */
			condition = build_condition_tree(args_bundle);
			break;
		}
		else if (!strcmp(current_argument, "!") || !strcmp(current_argument, "not"))
		{
			condition = make_empty_condition();
			condition->do_check = check_not;
			condition->data1 = create_condition_data(build_condition_node(args_bundle));
			condition->data1_content = CONDITION;
			break;
		}
		else if ((condition = try_parse_condition(current_argument, args_bundle)))
		{
			break;
		}
		else if (try_parse_action(current_argument, args_bundle))
		{
			/* do nothing */
		}
		else if (try_parse_option(current_argument, args_bundle))
		{
			/* do nothing */
		}
		else if (!strcmp(current_argument, ")"))
		{
			errx(1, ARG1_ERR_MSG, next_arg_index - 1, "')' was unexpected at this point");
			break;
		}
		else
		{
			errx(1, ARG2_ERR_MSG, next_arg_index - 1, "Unknown token", current_argument);
		}
	}
	return (condition);
}


condition_t *build_condition_tree(args_bundle_t *args_bundle)
{
	char *current_argument = NULL;
	condition_t *condition = NULL;
	condition_t *condition_buffer = NULL;
	condition_t *condition_temp = NULL;
	
	/* Fetch the first condition explicitly */
	condition = build_condition_node(args_bundle);
   
	if (!condition)
	{
		return (NULL);
	}
	
	while (next_arg_index < argument_count)
	{
		current_argument = argument_data[next_arg_index];
		next_arg_index++;
		
		if (!strcmp(current_argument, ")"))
		{
			break;
		}
		else if (!strcmp(current_argument, "or"))
		{
			condition_temp = build_condition_node(args_bundle);
			if (!condition_temp)
			{
				assert(next_arg_index >= argument_count);
				errx(1, ARG1_ERR_MSG, next_arg_index, "Expression expected after an 'or' operator");
				break;
			}
			
			if (!condition_buffer)
			{
				/* buffer is empty => fill the buffer */
				condition_buffer = condition_temp;
			}
			else
			{
				/* buffer is not empty => merge the buffer with main condition
					 via 'OR' operation and refill the buffer */
				condition = merge_condition_nodes(condition, condition_buffer, check_or);
				condition_buffer = condition_temp;
			}

			condition_temp = NULL;
		}
		else
		{
			/* implicit 'AND' operation between arguments */
			if (strcmp(current_argument, "and"))
			{
				/* current token isn't 'AND' => condition consumed => undo */
				next_arg_index--;
			}
			condition_temp = build_condition_node(args_bundle);
			
			if (!condition_temp)
			{
				assert(next_arg_index >= argument_count);
				/*errx(1, "Argument parsing error at next_arg_index %d: Expression expected after an 'and' operator", next_arg_index);*/
				break;
			}
			
			if (!condition_buffer)
			{
				/* buffer is empty => operation 'AND' is with left node */
				condition = merge_condition_nodes(condition, condition_temp, check_and);
			}
			else
			{
				/* buffer is not empty => operation 'AND' is with the right node */
				condition_buffer = merge_condition_nodes(condition_buffer, condition_temp, check_and);
			}
			
			condition_temp = NULL;
		}
	}
	
	if (condition_buffer) 
	{
		/* buffer isn't empty => merge buffer with main condition via 'OR' operation */
		condition = merge_condition_nodes(condition, condition_buffer, check_or);
		condition_buffer = NULL;
	}
	
	return (condition);
}


void
argument_range_check(char * expected)
{
	if (next_arg_index >= argument_count) 
	{
		errx(1, ARG1_ERR_MSG, next_arg_index, expected);
	}
}