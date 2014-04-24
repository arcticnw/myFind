#ifndef _CRAWLER_H_
#define _CRAWLER_H_

#include <sys/types.h>

#include "header.h"

struct node_p {
    ino_t nodeid;
    struct node_p * next;
};
typedef struct node_p node_t;

struct nodelist_p {
    int count;
    node_t * first;
    node_t * last;
};
typedef struct nodelist_p nodelist_t;

nodelist_t * initializeNodeList();
void disposeNodeList(nodelist_t * list);
int tryAddNode(nodelist_t * list, ino_t nodeid);

void crawl(const argsBundle_t argsBundle);
void crawlRecursive(const char * path, const argsBundle_t argsBundle, nodelist_t * list);

#endif