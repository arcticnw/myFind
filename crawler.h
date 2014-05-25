#ifndef _CRAWLER_H_
#define	_CRAWLER_H_

#include <sys/types.h>

#include "common.h"

/* this structure holds information about visited node ids and their relative */
/* paths and a pointer to next one */
struct node_p {
    ino_t node_id;
    char *local_name;
    struct node_p *next;
};
typedef struct node_p node_t;


/* this structure holds pointer to the first and last pointer of the node */
/* list and number of elements in this linked list */
struct node_list_p {
    int count;
    node_t *first;
    node_t *last;
};
typedef struct node_list_p node_list_t;


void
check_file(struct dirent *file_entry, char *local_path,
    const args_bundle_t *args_bundle, int depth, char *recurse);


/* this function initializes and returnd a node list */
node_list_t *
initialize_node_list();


/* this function deallocates the linked list and all linked nodes */
void
dispose_node_list(node_list_t *list);


/* this function searches the linked list for a node with given node_id; */
/* if a node is found, 0 is returned and a pointer to this node is stored */
/* in conflictingNode parameter; */
/* if there is no node with such node_id, 1 is returned, new node is */
/* created with this node_id and conflictingNode is pointed to this */
/* new node */
int
try_add_node(node_list_t *list, const char *local_name, const ino_t node_id,
    node_t ** conflictingNode);


/* this function goes through the linked list of actions and applies their */
/* do_action function-pointer on the file_info_bundle_t */
void
do_actions(const args_bundle_t *args_bundle, file_info_bundle_t file);


/* this function starts a search using given arguments */
void
crawl(const args_bundle_t *args_bundle);


/* this function starts a recursive search at given path, with given list of */
/* passed node_ids and given arguments */
void
crawl_recursive(const char *path, const args_bundle_t *args_bundle, int depth,
    node_list_t *list);

#endif