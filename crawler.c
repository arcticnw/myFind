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

#include "header.h"
#include "crawler.h"

extern int errno;

nodelist_t * initializeNodeList()
{
	nodelist_t * nodes = NULL;
	
	nodes = (nodelist_t*)malloc(sizeof(nodelist_t));
	if (!nodes)
	{
		err(1, "initializeNodeList - malloc (nodes)\n");
	}
	
	nodes->count = 0;
	nodes->first = NULL;
	nodes->last = NULL;
	
	return (nodes);
}

void disposeNodeList(nodelist_t * list)
{
	node_t * cNode;
	node_t * cNodeNext;
	
	if (!list)
	{
		return;
	}
	
	for (cNode = list->first; cNode;)
	{
		cNodeNext = cNode->next;
		free(cNode->localName);
		free(cNode);
		cNode = cNodeNext;
	}
	
	free(list);
}

int tryAddNode(nodelist_t * list, const char * localName, const ino_t nodeid, node_t ** nodeRef)
{
	node_t * newNode;
	node_t * cNode;

	/* check whether nodeid is already present in the list */
	for (cNode = list->first; cNode; cNode = cNode->next)
	{
		if (cNode->nodeid == nodeid)
		{
			*nodeRef = cNode;
			return (0);
		}
	}
	
	/* allocate a new node */
	newNode = (node_t*)malloc(sizeof(node_t));
	if (!newNode)
	{
		err(1, "tryAddNode - malloc (newNode)\n");
	}
	
	/* initialize new node */
	*nodeRef = newNode;
	newNode->nodeid = nodeid;
	newNode->next = NULL;
	newNode->localName = (char*)malloc(sizeof(char) * (strlen(localName) + 1));
	if (!newNode->localName)
	{
		err(1, "tryAddNode - malloc (localName)\n");
	}
	strcpy(newNode->localName, localName);
	
	/* insert node to the list */
	if (list->count == 0)
	{
		list->first = newNode;
	}
	else
	{
		list->last->next = newNode;
	}
	list->last = newNode;

	list->count++;
	
	return (1);
}

void crawl(const argsBundle_t argsBundle)
{
	nodelist_t * nodes = NULL;
	
	assert(argsBundle.action);

	/* initialize node list for path loop detection */
	nodes = initializeNodeList();
		
	/* start the search */
	crawlRecursive(argsBundle.path, argsBundle, nodes);
	
	disposeNodeList(nodes);
}

void doActions(const argsBundle_t argsBundle, file_t file)
{
	action_t * action;
	
	for(action = argsBundle.action; action; action = action->next)
	{
		action->doProcess(action, file);
	}
}

void crawlRecursive(const char * path, const argsBundle_t argsBundle, nodelist_t * list)
{
	DIR * dir;				 	/* current directory */
	DIR * subdir;				/* subdirectory */
	struct dirent * dirEntry;	/* current file name */
	struct stat dirEntryStat;	/* current file status  */
	char * localPath = NULL;	/* relative path to current file */
	int localPathLength;		/* (string) length of relative path */
	char * realPath = NULL;		/* absolute path to current file */
	char isLink;				/* current file is symlink */
	file_t f_entry;				/* current file information pack */
	int result;					/* result of matching with conditions */
	node_t * node;			 	/* node returned by tryAddNode */
	
	/* make sure we aren't in path loop */
	if (argsBundle.followLinks)
	{
		if (stat(path, &dirEntryStat))
		{
			fprintf(stderr, "Unable to access directory information %s: %s\n", path, strerror(errno));
			return;
		}
		if (argsBundle.followLinks && !tryAddNode(list, path, dirEntryStat.st_ino, &node))
		{
			fprintf(stderr, "File system loop detected: %s was already visited in %s\n", path, node->localName);
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
	while ((dirEntry = readdir(dir)))
	{ 
		/* skip self and parent directory */
		if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, ".."))
		{
			continue;
		}
		
		/* skip hidden */
		if (argsBundle.ignoreHidden && dirEntry->d_name[0] == '.')
		{
			continue;
		}
	
		/* get relative path */
		localPathLength = strlen(path) + strlen(dirEntry->d_name) + 2;
		localPath = malloc(localPathLength);
		snprintf(localPath, localPathLength, "%s%s", path, dirEntry->d_name);
		
		/* get absolute path */
		realPath = realpath(localPath, (char*)NULL);
		
		/* get file status */
		if (argsBundle.followLinks)
		{
			if (stat(localPath, &dirEntryStat))
			{
				fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
				isLink = 0;
				goto cleanupAndContinue; /* ask if allowed to use goto */
			}
		}
		else
		{
			if (lstat(localPath, &dirEntryStat))
			{
				fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
				isLink = 0;
				goto cleanupAndContinue; /* ask if allowed to use goto */
			}
		}

		isLink = (S_ISLNK(dirEntryStat.st_mode) != 0);
		
		/* prepare file infomation pack */
		f_entry.dirEntry = dirEntry;
		f_entry.localPath = localPath;
		f_entry.realPath = realPath;
		f_entry.dirEntryStat = dirEntryStat;
				
		if (argsBundle.condition)
		{
			/* match file with find conditions */
			result = argsBundle.condition->doCheck(argsBundle.condition, f_entry);
		}
		else
		{
			/* no condition => match everything */
			result = 1;
		}
			
		/* if matching => apply actions */
		if (result)
		{
			doActions(argsBundle, f_entry);
		}
		
		/* if file isn't symlink and following is disabled, attempt to 
			 open file as directory and traverse it */
		
		if ((argsBundle.followLinks || !isLink) &&
			(subdir = opendir(localPath)))
		{
			closedir(subdir);
			crawlRecursive(localPath, argsBundle, list);
		}
			
cleanupAndContinue: /* ask if allowed to use goto */
			
		/* dealloc paths */
		free(realPath);
		free(localPath);
	}
	
	/* close directory */
	closedir(dir);
}