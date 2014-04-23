#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>

#include "header.h"
#include "crawler.h"

int crawl(char * path, condition_t * condition)
{
    DIR * dir;
    struct dirent * dirEntry;
    file_t f_entry;
    char * localPath = NULL;
    int localPathLength;
    char * realPath = NULL;
    int result;
    
    if (!(dir = opendir(path)))
    {
        fprintf(stderr, "Unable to open directory %s\n", path);
        return (0);
    }
    
    while (dirEntry = readdir(dir))
    {
        localPathLength = strlen(path) + strlen(dirEntry->d_name) + 2;
        localPath = malloc(localPathLength);
        sprintf(localPath, "%s/%s", path, dirEntry->d_name);
        
        realPath = realpath(localPath, NULL);
        
        f_entry.dirEntry = dirEntry;
        f_entry.localPath = localPath;
        f_entry.realPath = realPath;
        
        result = condition.process(condition->data1, condition->data2, f_entry);
        
        if (result)
        {
            printf(realPath);
        }
        
        free(realPath);
        free(localPath);
    }
    
    closedir(path);
    
    return (0);
}