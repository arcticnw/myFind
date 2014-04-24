#include <stdlib.h>
#include <unistd.h> /* realpath */
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>

#include "header.h"
#include "crawler.h"

extern int errno;

void crawl(argsBundle_t argsBundle)
{
    nodelist_t * nodes = NULL;
    node_t * cNode = NULL;
    node_t * cNodeNext = NULL;
    
    nodes = (nodelist_t*)malloc(sizeof(nodelist_t));
    nodes->count = 0;
    nodes->first = NULL;
    
    crawlRecursive(argsBungle.path, argsBundle, nodes);
    
    for (cNode = nodes->first; cNode; cNodeNext = cNode->next)
    {
	free(cNode);
	cNode = cNodeNext;
    }
    free(nodes);
}

int tryAddNode(nodelist_t * list, ino_t nodeid)
{
    node_t * newNode;
    node_t * cNode;

    for (cNode = list->first; cNode; )
    {
	if (cNode->nodeid == nodeid)
	{
	    return (0);
	}
    }
    
    newNode = (node_t*)malloc(sizeof(node_t));
    newNode->nodeid = nodeid;
    newNode->next = NULL;
    list->count++;
    
    if (list->count == 0)
    {
	list->first = newNode;
	list->last = newNode;
    }
    else
    {
	list->last->next = newNode;
	list->last = newNode;
    }
    
    return (1);
}

void crawlRecursive(char * path, argsBundle_t argsBundle, nodelist_t * directoryNodes)
{
    DIR * dir;                 /* current directory */
    DIR * subdir;              /* subdirectory */
    struct dirent * dirEntry;  /* current file name */
    struct stat dirEntryStat;  /* current file status  */
    char * localPath = NULL;   /* relative path to current file */
    int localPathLength;       /* (string) length of relative path */
    char * realPath = NULL;    /* absolute path to current file */
    char isLink;               /* current file is symlink */
    char isStatFetched;        /* is file status fetched */
    file_t f_entry;            /* current file information pack */
    int result;                /* result of matching with conditions */
    
/*    printf("opening %s\n", path);*/
    
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
        if (dirEntry->d_name[0] == '.') /* DBG */
        {
	    continue;
        }
    
        /* get relative path */
        localPathLength = strlen(path) + strlen(dirEntry->d_name) + 2;
        localPath = malloc(localPathLength);
        sprintf(localPath, "%s/%s", path, dirEntry->d_name);
        
	/* get absolute path */
        realPath = realpath(localPath, (char*)NULL);
        
	/* get file status */
	if (argsBundle.linkResolution == FOLLOW)
	{
	    if (stat(localPath, &dirEntryStat))
	    {
		fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
		isLink = 0;
		isStatFetched = 0;
	    }
	    else
	    {
		isStatFetched = 1;
	    }
	}
	else /*if (argsBundle.linkResolution == NOFOLLOW)*/
	{
	    if (lstat(localPath, &dirEntryStat))
	    {
		fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
		isLink = 0;
		isStatFetched = 0;
	    }
	    else
	    {
		isStatFetched = 1;
	    }
	}

	if (isStatFetched)
	{
	    switch (dirEntryStat.st_mode & S_IFMT) {
		case S_IFBLK:  printf("block device");            break;
		case S_IFCHR:  printf("character device");        break;
		case S_IFDIR:  printf("directory");               break;
		case S_IFIFO:  printf("FIFO/pipe");               break;
		case S_IFLNK:  printf("symlink");                 break;
		case S_IFREG:  printf("regular file");            break;
		case S_IFSOCK: printf("socket");                  break;
		default:       printf("unknown");                break;
	    }

	    
	    if (S_ISLNK(dirEntryStat.st_mode))
	    {
		isLink = 1;
	    }

	    if (argsBundle.condition)
	    {
		/*printf("making file pack\n");*/
		/* prepare file infomation pack */
		f_entry.dirEntry = dirEntry;
		f_entry.localPath = localPath;
		f_entry.realPath = realPath;
		f_entry.dirEntryStat = dirEntryStat;
            
		/* match file with find conditions */
		/*printf("testing file\n");*/
		result = argsBundle.condition->process(argsBundle.condition, f_entry);
	    }
	    else
	    {
		result = 1;
	    }
            
            /* if matching => apply actions */
            if (result)
            {
		/*printf("running action\n");*/
                if (!argsBundle.action)
                {
		    printf("THIS ONE");
/*                  printf("%s\n", localPath);*/
                }
                else
                {
                    err(9, "NYI");
                }
            }
        }
        
	/*printf("checking directory\n");*/

        /* if file isn't symlink and following is disabled, attempt to 
             open file as directory and traverse it */
        
        if ((argsBundle.linkResolution == FOLLOW || !isLink) &&
            (subdir = opendir(localPath)))
        {
    	    printf(" ENTERING \n");
            closedir(subdir);
            crawlRecursive(localPath, argsBundle);
        }
        
	/*printf("deallocating\n");*/
        /* dealloc paths */
        free(realPath);
        free(localPath);
        
        printf ("\n");
    }
    
    
    /* close directory */
    closedir(dir);
}