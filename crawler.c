#include <stdlib.h>
#include <unistd.h> /* realpath */
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "header.h"
#include "crawler.h"

extern int errno;

int crawl(argsBundle_t argsBundle)
{
    crawl(argsBundle.path, argsBundle);
}
void crawl(char * path, argsBundle_t argsBundle)
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
    
        /* get relative path */
        localPathLength = strlen(path) + strlen(dirEntry->d_name) + 2;
        localPath = malloc(localPathLength);
        sprintf(localPath, "%s/%s", path, dirEntry->d_name);
        
        /* get absolute path */
        realPath = realpath(localPath, (char*)NULL);
        
        /* get file status */
        if (argsBundle.linkResolution == FOLLOW && stat(realPath, &dirEntryStat))
        {
            fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
            dirEntryStat = NULL;
            isLink = 0;
        }

        /* TODO: broken link */
        
        if (argsBundle.linkResolution == NOFOLLOW && lstat(realPath, &dirEntryStat))
        {
            fprintf(stderr, "Unable to access file %s: %s\n", localPath, strerror(errno));
            dirEntryStat = NULL;
            isLink = 0;
        }
        
        if (dirEntryStat)
        {
            /* prepare file infomation pack */
            f_entry.dirEntry = dirEntry;
            f_entry.localPath = localPath;
            f_entry.realPath = realPath;
            f_entry.dirEntryStat = dirEntryStat;
            
            isLink = S_ISLNK(dirEntry);
            /* match file with find conditions */
            result = argsBundle.condition->process(condition, f_entry);
            
            /* if matching => apply actions */
            if (result)
            {
                if (!argsBundle.action)
                {
                    printf("%s\n", localPath);
                }
                else
                {
                    err(9, "NYI");
                }
            }
        }
        
        /* if file isn't symlink and following is disabled, attempt to 
             open file as directory and traverse it */
        
        if ((argsBundle.linkResolution == FOLLOW || !isLink) &&
            (subdir = opendir(localPath)))
        {
            closedir(localPath);
            crawl(localPath, argsBundle);
        }
        
        /* dealloc paths */
        free(realPath);
        free(localPath);
    }
    
    /* close directory */
    closedir(dir);
}