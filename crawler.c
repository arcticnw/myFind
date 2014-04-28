#include <stdlib.h>
#include <unistd.h> /* realpath */
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <assert.h>

#include "common.h"
#include "crawler.h"

node_list_t *initialize_node_list()
{
	node_list_t *nodes = NULL;
	
	nodes = malloc(sizeof(node_list_t));
	if (!nodes)
	{
		err(1, "initialize_node_list - malloc (nodes)\n");
	}
	
	nodes->count = 0;
	nodes->first = NULL;
	nodes->last = NULL;
	
	return (nodes);
}

void dispose_node_list(node_list_t *list)
{
	node_t *current_node;
	node_t *current_node_next;
	
	if (!list)
	{
		return;
	}
	
	for (current_node = list->first; current_node;)
	{
		current_node_next = current_node->next;
		free(current_node->local_name);
		free(current_node);
		current_node = current_node_next;
	}
	
	free(list);
}

int try_add_node(node_list_t *list, const char *local_name, const ino_t node_id, node_t ** nodeRef)
{
	node_t *new_node;
	node_t *current_node;

	/* check whether node_id is already present in the list */
	for (current_node = list->first; current_node; current_node = current_node->next)
	{
		if (current_node->node_id == node_id)
		{
			*nodeRef = current_node;
			return (0);
		}
	}
	
	/* allocate a new node */
	new_node = malloc(sizeof(node_t));
	if (!new_node)
	{
		err(1, "try_add_node - malloc (new_node)\n");
	}
	
	/* initialize new node */
	*nodeRef = new_node;
	new_node->node_id = node_id;
	new_node->next = NULL;
	new_node->local_name = malloc(sizeof(char) * (strlen(local_name) + 1));
	if (!new_node->local_name)
	{
		err(1, "try_add_node - malloc (local_name)\n");
	}
	strcpy(new_node->local_name, local_name);
	
	/* insert node to the list */
	if (list->count == 0)
	{
		list->first = new_node;
	}
	else
	{
		list->last->next = new_node;
	}
	list->last = new_node;

	list->count++;
	
	return (1);
}

void crawl(const args_bundle_t *args_bundle)
{
	node_list_t *nodes = NULL;
	
	assert(args_bundle->action);

	/* initialize node list for path loop detection */
	nodes = initialize_node_list();
		
	/* start the search */
	crawl_recursive(args_bundle->path, args_bundle, nodes);
	
	dispose_node_list(nodes);
}

void do_actions(const args_bundle_t *args_bundle, file_t file)
{
	action_t *action;
	
	for(action = args_bundle->action; action; action = action->next)
	{
		action->do_action(action, file);
	}
}

void crawl_recursive(const char *path, const args_bundle_t *args_bundle, node_list_t *list)
{
	DIR * dir;                  /* current directory */
	DIR * subdir;               /* subdirectory */
	struct dirent * dir_entry;  /* current file name */
	struct stat dir_entry_stat; /* current file status  */
	char *local_path = NULL;    /* relative path to current file */
	int local_path_length;      /* (string) length of relative path */
	char *real_path = NULL;     /* absolute path to current file */
	char isLink;                /* current file is symlink */
	file_t file_entry;          /* current file information pack */
	int result;                 /* result of matching with conditions */
	node_t *node;               /* node returned by try_add_node */
	
	/* make sure we aren't in path loop */
	if (args_bundle->follow_links)
	{
		if (stat(path, &dir_entry_stat))
		{
			fprintf(stderr, "Unable to access directory information %s: %s\n", path, strerror(errno));
			return;
		}
		if (args_bundle->follow_links && !try_add_node(list, path, dir_entry_stat.st_ino, &node))
		{
			fprintf(stderr, "File system loop detected: %s was already visited in %s\n", path, node->local_name);
			return;
		}
	}
	
	/* open directory */
	if (!(dir = opendir(path)))
	{
		fprintf(stderr, "Unable to open directory %s: %s\n", path, strerror(errno));
		return;
	}
	
	/* traverse directory */
	while ((dir_entry = readdir(dir)))
	{ 
		/* skip self and parent directory */
		if (!strcmp(dir_entry->d_name, ".") || !strcmp(dir_entry->d_name, ".."))
		{
			continue;
		}
		
		/* skip hidden */
		if (args_bundle->ignore_hidden && dir_entry->d_name[0] == '.')
		{
			continue;
		}
	
		/* get relative path */
		local_path_length = strlen(path) + strlen(dir_entry->d_name) + 2;
		local_path = malloc(local_path_length);
		snprintf(local_path, local_path_length, "%s/s", path, dir_entry->d_name);
		
		/* get absolute path */
		real_path = realpath(local_path, (char*)NULL);
		
		/* get file status */
		if (args_bundle->follow_links)
		{
			if (stat(local_path, &dir_entry_stat))
			{
				fprintf(stderr, "Unable to access file %s: %s\n", local_path, strerror(errno));
				isLink = 0;
				goto cleanupAndContinue; /* ask if allowed to use goto */
			}
		}
		else
		{
			if (lstat(local_path, &dir_entry_stat))
			{
				fprintf(stderr, "Unable to access file %s: %s\n", local_path, strerror(errno));
				isLink = 0;
				goto cleanupAndContinue; /* ask if allowed to use goto */
			}
		}

		isLink = (S_ISLNK(dir_entry_stat.st_mode) != 0);
		
		/* prepare file infomation pack */
		file_entry.dir_entry = dir_entry;
		file_entry.local_path = local_path;
		file_entry.real_path = real_path;
		file_entry.dir_entry_stat = dir_entry_stat;
				
		if (args_bundle->condition)
		{
			/* match file with find conditions */
			result = args_bundle->condition->do_check(args_bundle->condition, file_entry);
		}
		else
		{
			/* no condition => match everything */
			result = 1;
		}
			
		/* if matching => apply actions */
		if (result)
		{
			do_actions(args_bundle, file_entry);
		}
		
		/* if file isn't symlink and link-following is disabled, attempt to 
			 open file as directory and traverse it */
		
		if ((args_bundle->follow_links || !isLink) &&
			(subdir = opendir(local_path)))
		{
			closedir(subdir);
			crawl_recursive(local_path, args_bundle, list);
		}
			
cleanupAndContinue: /* ask if allowed to use goto */
			
		/* dealloc paths */
		free(real_path);
		free(local_path);
	}
	
	/* close directory */
	closedir(dir);
}