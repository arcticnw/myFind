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
    
    return (nodes);
}

void disposeNodeList(nodelist_t * list)
{
    node_t * cNode;
    note_t * cNodeNext;
    
    if (!list)
    {
        return;
    }
    
    for (cNode = nodes->first; cNode;)
    {
        cNodeNext = cNode->next
        free(cNode);
        cNode = cNodeNext;
    }
    
    free(list);
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
    }
    else
    {
        list->last->next = newNode;
    }
    list->last = newNode;
    
    return (1);
}

void crawl(const argsBundle_t argsBundle)
{
    nodelist_t * nodes = NULL;
    
    nodes = initializeNodeList();
    
    crawlRecursive(argsBungle.path, argsBundle, nodes);
    
    disposeNodeList(nodes);
}

void crawlRecursive(const char * path, const argsBundle_t argsBundle, nodelist_t * list)
{
    DIR * dir;                 /* current directory */
    DIR * subdir;              /* subdirectory */
    struct dirent * dirEntry;  /* current file name */
    struct stat dirEntryStat;  /* current file status  */
    char * localPath = NULL;   /* relative path to current file */
    int localPathLength;       /* (string) length of relative path */
    char * realPath = NULL;    /* absolute path to current file */
    char isLink;               /* current file is symlink */
    file_t f_entry;            /* current file information pack */
    int result;                /* result of matching with conditions */
    
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
        
        if (argsBundle.ignoreHidden && dirEntry->d_name[0] == '.')
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
        if (argsBundle.followLinks)
        {
            if (stat(localPath, &dirEntryStat))
            {
                fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
                isLink = 0;
                goto cleanupAndContinue; /* ask if allowed to use goto */
            }
        }
        else /*if (!argsBundle.followLinks)*/
        {
            if (lstat(localPath, &dirEntryStat))
            {
                fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
                isLink = 0;
                goto cleanupAndContinue; /* ask if allowed to use goto */
            }
        }

        if (S_ISLNK(dirEntryStat.st_mode))
        {
            isLink = 1;
        }

        if (argsBundle.condition)
        {
            /* prepare file infomation pack */
            f_entry.dirEntry = dirEntry;
            f_entry.localPath = localPath;
            f_entry.realPath = realPath;
            f_entry.dirEntryStat = dirEntryStat;
                
            /* match file with find conditions */
            result = argsBundle.condition->process(argsBundle.condition, f_entry);
        }
        else
        {
            /* no condition => match everything */
            result = 1;
        }
            
        /* if matching => apply actions */
        if (result)
        {
            if (!argsBundle.action)
            {
                printf("%s\n", localPath);
            }
            else
            {
                err(9, "NYI\n");
            }
        }
        
            
        /* if file isn't symlink and following is disabled, attempt to 
             open file as directory and traverse it */
        
        if ((argsBundle.followLinks || !isLink) &&
            (subdir = opendir(localPath)))
        {
            closedir(subdir);
            if (argsBundle.followLinks && !tryAddNode(list, dirEntryStat->st_ino))
            {
                fprintf(stderr, "File system loop detected: %s\n", localPath);
            }
            else
            {
                crawlRecursive(localPath, argsBundle, list);
            }
        }
            
cleanupAndContinue: /* ask if allowed to use goto */
            
        /* dealloc paths */
        free(realPath);
        free(localPath);
    }
    
    /* close directory */
    closedir(dir);
}