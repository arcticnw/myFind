#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "common.h"
#include "crawler.h"

node_list_t *
initialize_node_list() {
	node_list_t *nodes = NULL;

	nodes = malloc(sizeof (node_list_t));
	if (!nodes) {
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}

	nodes->count = 0;
	nodes->first = NULL;
	nodes->last = NULL;

	return (nodes);
}

void
dispose_node_list(node_list_t *list) {
	node_t *current_node;
	node_t *current_node_next;

	if (!list) {
		return;
	}

	for (current_node = list->first; current_node;) {
		current_node_next = current_node->next;
		free(current_node->local_name);
		free(current_node);
		current_node = current_node_next;
	}

	free(list);
}

int
try_add_node(node_list_t *list, const char *local_name, const ino_t node_id,
    node_t ** nodeRef) {
	node_t *new_node;
	node_t *current_node;

	/* check whether node_id is already present in the list */
	for (current_node = list->first;
	    current_node;
	    current_node = current_node->next) {
		if (current_node->node_id == node_id) {
			*nodeRef = current_node;
			return (0);
		}
	}

	/* allocate a new node */
	new_node = malloc(sizeof (node_t));
	if (!new_node) {
		errx(127, MALLOC_ERR_MSG, strerror(errno));
	}

	/* initialize new node */
	*nodeRef = new_node;
	new_node->node_id = node_id;
	new_node->next = NULL;
	new_node->local_name = copy_string(local_name);

	/* insert node to the list */
	if (0 == list->count) {
		list->first = new_node;
	}
	else {
		list->last->next = new_node;
	}
	list->last = new_node;

	list->count++;

	return (1);
}

void
do_actions(const args_bundle_t *args_bundle, file_info_bundle_t file) {
	action_t *action;

	for (action = args_bundle->action; action; action = action->next) {
		action->do_action(action, file);
	}
}

void
crawl(const args_bundle_t *args_bundle) {
	node_list_t *nodes = NULL;

	assert(args_bundle->action);

	/* initialize node list for path loop detection */
	nodes = initialize_node_list();

	/* start the search */
	crawl_recursive(args_bundle->path, args_bundle, 0, nodes);

	dispose_node_list(nodes);
}

void
crawl_recursive(const char *path, const args_bundle_t *args_bundle, int depth,
    node_list_t *list) {
	DIR * dir; /* current directory */
	DIR * subdir; /* subdirectory */
	struct dirent * file_entry; /* current file name */
	struct stat file_entry_stat; /* current file status  */
	char *local_path = NULL; /* relative path to current file */
	int local_path_length; /* (string) length of relative path */
	char *real_path = NULL; /* absolute path to current file */
	char isLink; /* current file is symlink */
	file_info_bundle_t file_info; /* current file information pack */
	int result; /* result of matching with conditions */
	node_t *node; /* node returned by try_add_node */
	char validDepth; /* current depth is within specified bounds */

	validDepth =
	    args_bundle->min_depth < depth &&
	    (args_bundle->max_depth == -1 || depth < args_bundle->max_depth);

	/* make sure we aren't in path loop */
	/* check only if link-following is allowed */
	if (args_bundle->follow_links) {
		if (stat(path, &file_entry_stat)) {
			fprintf(stderr, DIR_ACCESS_WRN_MSG,
			    path, strerror(errno));
			return;
		}
		if (args_bundle->follow_links && !try_add_node(list, path,
		    file_entry_stat.st_ino, &node)) {
			fprintf(stderr, DIR_LOOP_WRN_MSG,
			    path, node->local_name);
			return;
		}
	}

	/* open directory */
	if (!(dir = opendir(path))) {
		fprintf(stderr, DIR_ACCESS_WRN_MSG, path, strerror(errno));
		return;
	}

	/* traverse directory */
	while ((file_entry = readdir(dir))) {
		/* skip self and parent directory */
		if (0 == strcmp(file_entry->d_name, ".") ||
		    0 == strcmp(file_entry->d_name, "..")) {
			continue;
		}

		/* skip hidden */
		if (args_bundle->ignore_hidden &&
		    file_entry->d_name[0] == '.') {
			continue;
		}

		/* get relative path */
		local_path_length = strlen(path) +
		    strlen(file_entry->d_name) + 2;
		local_path = malloc(sizeof (char) * local_path_length);
		if (!local_path) {
			errx(127, MALLOC_ERR_MSG, strerror(errno));
		}
		snprintf(local_path, local_path_length, "%s/%s",
		    path, file_entry->d_name);

		/* get absolute path */
		real_path = realpath(local_path, (char*)NULL);

		/* get file status */
		if (args_bundle->follow_links &&
		    stat(local_path, &file_entry_stat)) {
			fprintf(stderr, FILE_ACCESS_WRN_MSG,
			    local_path, strerror(errno));
			isLink = 0;
			goto cleanupAndContinue;
		}

		if (!args_bundle->follow_links &&
		    lstat(local_path, &file_entry_stat)) {
			fprintf(stderr, FILE_ACCESS_WRN_MSG,
			    local_path, strerror(errno));
			isLink = 0;
			goto cleanupAndContinue;
		}

		isLink = (S_ISLNK(file_entry_stat.st_mode) != 0);

		/* start file testing only if the min depth has been reached */
		if (validDepth) {

			/* prepare file infomation pack */
			file_info.file_entry = file_entry;
			file_info.local_path = local_path;
			file_info.real_path = real_path;
			file_info.file_entry_stat = file_entry_stat;
			file_info.time_now = args_bundle->time_now;

			if (args_bundle->condition) {
				/* match file with find conditions */
				result = args_bundle->condition->do_check(
				    args_bundle->condition, file_info);
			}
			else {
				/* no condition => match everything */
				result = 1;
			}

			/* if matching => apply actions */
			if (result) {
				do_actions(args_bundle, file_info);
			}

		}

		/* if file isn't symlink and link-following is off, attempt to
			 open file as directory and traverse it */

		if ((args_bundle->follow_links || !isLink) &&
		    (args_bundle->max_depth == -1 ||
		    depth + 1 < args_bundle->max_depth) &&
		    (subdir = opendir(local_path))) {
			closedir(subdir);
			crawl_recursive(local_path, args_bundle, depth+1, list);
		}

cleanupAndContinue: /* ask if allowed to use goto */

		/* dealloc paths */
		free(real_path);
		free(local_path);
	}

	/* close directory */
	closedir(dir);
}