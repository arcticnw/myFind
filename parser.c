#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "parser.h"
#include "checker.h"
#include "action.h"

int current_position;
int argument_count;
char **argument_data;

args_bundle_t *parse_arguments(int argc, char **argv)
{
	args_bundle_t *args_bundle;

	argument_count = argc;
	argument_data = argv;
	
	args_bundle_t = initialize_args_bundle();
	
	args_bundle->path = argument_data[1];
	
	current_position = 2;
	args_bundle->condition = build_condition_tree(args_bundle);
	
	if (!args_bundle->action)
	{
		append_action(args_bundle, create_default_action());
	}
	
	argument_count = 0;
	argument_data = NULL;
	
	return (args_bundle);
}

args_bundle_t *initialize_args_bundle()
{
	args_bundle_t *args_bundle;
	args_bundle = malloc(sizeof(args_bundle_t));
	if (!args_bundle)
	{
		errx(127, "Failed to allocate memory: %s", strerror(errno));
	}
	args_bundle->follow_links = 0;
	args_bundle->ignore_hidden = 1;
	args_bundle->path = NULL;
	args_bundle->condition = NULL;
	args_bundle->action = NULL;
}

action_t *create_default_action()
{
	action_t *action;
	
	action = create_action();
	action->do_action = do_print;
	
	return (action);
}

void dispose_args_bundle(args_bundle_t *args_bundle)
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

void dispose_condition(condition_t *condition)
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

void dispose_action(action_t *action)
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

data_t create_string_data(const char *original_data)
{
	data_t data;
	
	data.string_data = malloc(sizeof(char) * (strlen(original_data) + 1));
	if (!data.string_data)
	{
		errx(127, "Failed to allocate memory: %s", strerror(errno));
	}
	strcpy(data.string_data, original_data);
	
	return (data);
}

data_t create_int_data(char *original_data, signed char *comparison)
{
	char *parse_data = original_data;
	int value;
	data_t data;
	
	assert(strlen(original_data));
	
	if (original_data[0] == '+')
	{
		parse_data++;
		*comparison = -1;
	}
	else if (original_data[0] == '-')
	{
		parse_data++;
		*comparison = +1;
	}
	else
	{
		*comparison = 0;
	}
	
	value = atoi(parse_data);
	
	data.int_data = value;
	
	return (data);
}

data_t create_condition_data(condition_t *condition)
{
	data_t data;

	data.condition_data = condition;
	
	return (data);
}


condition_t *merge_condition_nodes(condition_t *condition1, condition_t *condition2, chech_t checker)
{
	condition_t *condition = make_empty_condition();
	condition->do_check = checker;
	condition->data1 = create_condition_data(condition1);
	condition->data1_content = CONDITION;
	condition->data2 = create_condition_data(condition2);
	condition->data2_content = CONDITION;
	
	return (condition);
}


condition_t *make_empty_condition()
{
	condition_t *condition = malloc(sizeof(condition_t));
	if (!condition)
	{
		errx(127, "Failed to allocate memory: %s", strerror(errno));
	}
	
	condition->do_check = check_true;
	condition->data1.int_data = 0;
	condition->data1_content = NONE;
	condition->data2.int_data = 0;
	condition->data2_content = NONE;
	
	return (condition);
}

condition_t *make_string_condition(chech_t checker)
{
	condition_t *condition;

	if (current_position >= argument_count) 
	{ 
		errx(1, "Argument parsing error at current_position %d: String argument missing", current_position);
	}

	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_string_data(argument_data[current_position]);
	condition->data1_content = STRING;
	current_position++;

	return (condition);
}

condition_t *make_string_string_condition(chech_t checker)
{
	condition_t *condition;

	if (current_position >= argument_count) 
	{ 
		errx(1, "Argument parsing error at current_position %d: String argument missing", current_position);
	}

	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_string_data(argument_data[current_position]);
	condition->data1_content = STRING;
	current_position++;
	
	condition->data2 = create_string_data(argument_data[current_position]);
	condition->data2_content = STRING;
	current_position++;

	return (condition);
}

condition_t *make_int_condition(chech_t checker)
{
	condition_t *condition;

	if (current_position >= argument_count) 
	{ 
		errx(1, "Argument parsing error at current_position %d: Integer argument missing", current_position);
	}
	
	condition = make_empty_condition();
	condition->do_check = checker;
	
	condition->data1 = create_int_data(argument_data[current_position], &(condition->params.compare_method));
	condition->data1_content = INT;
	
	current_position++;

	return (condition); 
}

action_t *create_action()
{
	action_t *action;
	action = malloc(sizeof(action_t));
	if (!action)
	{
		errx(127, "Failed to allocate memory: %s", strerror(errno));
	}
	action->do_action = NULL;
	action->param_count = -1;
	action->next = NULL;
	action->params = NULL;
	return (action);
}
void append_action(args_bundle_t *args_bundle, action_t *action)
{
	action_t *cAction;
	assert(args_bundle);
	
	/* if there's no action yet, append directly */
	if (!args_bundle->action)
	{
		args_bundle->action = action;
		return;
	}
	
	/* find the last action */
	for(cAction = args_bundle->action; cAction->next; cAction = cAction->next) ;
	
	cAction->next = action;
}

condition_t *try_parse_condition(char *current_argument, args_bundle_t *args_bundle)
{
	condition_t *condition = NULL;
	
	if (!strcmp(current_argument, "name"))
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
	else if (!strcmp(current_argument, "true"))
	{
		condition = make_empty_condition();
		condition->do_check = check_true;
	}
	else if (!strcmp(current_argument, "false	"))
	{
		condition = make_empty_condition();
		condition->do_check = check_false;
	}
	return (condition);
}
int try_parse_action(char *current_argument, args_bundle_t *args_bundle)
{
	action_t *action;
	int i;
	int startPosition;
	int endPosition;
	int counter;
	
	if (!strcmp(current_argument, "exec"))
	{
		action = create_action();
		action->do_action = do_execute;
		
		startPosition = current_position;
		for (counter = 0; strcmp(current_argument, ";"); counter++)
		{
			while (current_position >= argument_count) 
			{ 
				errx(1, "Argument parsing error at current_position %d: ';' missing", current_position);
			}
			current_argument = argument_data[current_position];
			current_position++;
		}
		/* the first cycle is comparing 'exec' to ';' => don't count that' */
		counter--;
		action->param_count = counter;
		
		if (counter)
		{
			endPosition = current_position;
			current_position = startPosition;
			
			action->params = malloc (sizeof(char*) * counter);
			if (!action->params)
			{
				errx(127, "Failed to allocate memory: %s", strerror(errno));
			}
			for(i = 0; i < counter; i++)
			{
				current_argument = argument_data[current_position];
				current_position++;
				
				action->params[i] = malloc(sizeof(char) * (strlen(current_argument) + 1));
				if (!action->params[i])
				{
					errx(127, "Failed to allocate memory: %s", strerror(errno));
				}
				strcpy(action->params[i], current_argument);
			}
			
			current_position = endPosition;
		}
		
		append_action(args_bundle, action);
	}
	else if (!strcmp(current_argument, "print"))
	{
		action = create_action();
		action->do_action = do_print;
		append_action(args_bundle, action);
	}
	else
	{
		return (0);
	}
	return (1);
}
int try_parse_option(char *current_argument, args_bundle_t *args_bundle)
{
	if (!strcmp(current_argument, "-follow"))
	{
		args_bundle->follow_links = 1;
	}
	else if (!strcmp(current_argument, "-nofollow"))
	{
		args_bundle->follow_links = 0;
	}
	else if (!strcmp(current_argument, "-ignore-hidden"))
	{
		args_bundle->ignore_hidden = 1;
	}
	else if (!strcmp(current_argument, "-noignore-hidden"))
	{
		args_bundle->ignore_hidden = 0;
	}
	else
	{
		return (0);
	}
	return (1);
}

condition_t *build_condition_node(args_bundle_t *args_bundle)
{
	char *current_argument;
	condition_t *condition = NULL;
	
	while (current_position < argument_count)
	{
		current_argument = argument_data[current_position];
		current_position++;
		
		if (!strcmp(current_argument, "("))
		{
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
			errx(1, "Argument parsing error at current_position %d: ')' was unexpected at this point", current_position - 1);
			break;
		}
		else
		{
			errx(1, "Argument parsing error at current_position %d: unknown token '%s'", current_position - 1, current_argument);
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
	
	while (current_position < argument_count)
	{
		current_argument = argument_data[current_position];
		current_position++;
		
		if (!strcmp(current_argument, ")"))
		{
			break;
		}
		else if (!strcmp(current_argument, "or"))
		{
			condition_temp = build_condition_node(args_bundle);
			if (!condition_temp)
			{
				assert(current_position >= argument_count);
				errx(1, "Argument parsing error at current_position %d: expression expected after 'or' operator", current_position);
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
				current_position--;
			}
			condition_temp = build_condition_node(args_bundle);
			
			if (!condition_temp)
			{
				assert(current_position >= argument_count);
				/* fprintf(stderr, "build_condition_tree - binary operator 'and' used without second operand\n"); */
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