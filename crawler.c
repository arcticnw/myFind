#include <stdlib.h>
#include <unistd.h> /* realpath */
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
    struct stat dirEntryStat;
    char * localPath = NULL;
    int localPathLength;
    char * realPath = NULL;
    file_t f_entry;
    int result;
    
    if (!(dir = opendir(path)))
    {
        fprintf(stderr, "Unable to open directory %s\n", path);
        return (0);
    }
    
    while ((dirEntry = readdir(dir)))
    {
        if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, ".."))
        {
            continue;
        }
    
        localPathLength = strlen(path) + strlen(dirEntry->d_name) + 2;
        localPath = malloc(localPathLength);
        sprintf(localPath, "%s/%s", path, dirEntry->d_name);
        
        realPath = realpath(localPath, (char*)NULL);
        
        if (lstat(realPath, &dirEntryStat))
        {
            fprintf(stderr, "Unable to access file %s\n", realPath);
        }
        else
        {
            f_entry.dirEntry = dirEntry;
            f_entry.localPath = localPath;
            f_entry.realPath = realPath;
            f_entry.dirEntryStat = dirEntryStat;
            
            result = condition->process(condition, f_entry);
            
            if (result)
            {
                printf("%s\n", realPath);
            }
        }
        
        free(realPath);
        free(localPath);
    }
    
    closedir(dir);
    
    return (0);
}