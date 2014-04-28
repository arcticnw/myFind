#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "parser.h"
#include "crawler.h"
#include "debug.h"

void usage(char *name)
{
    printf("Usage: %s path [options] [expression [expression [..]]]\n"
		"\n"
		"where [options] are\n"
		"-follow\n"
		"-nofollow\n"
		"-ignore-hidden\n"
		"-noignore-hidden\n"
		"\n"
		"where [expression] is\n"
		"name filename\n"
		"iname filename\n"
		"\n"
		"and\n"
		"or\n"
		"(\n"
		")\n"
		"not\n"
		"!\n"
		"\n",
		name);
}

int main(int argc, char **argv)
{
    args_bundle_t *args_bundle;
    
    printf("hello\n");
    
    if (argc < 2)
    {
        usage(argv[0]);
        return 0;
    }
    
    printf("parse\n");
    
    args_bundle = parse_arguments(argc, argv);
    
    dumpArgsBundle(args_bundle);
    
    printf("crawl\n");
    crawl(args_bundle);
    
    printf("dispose\n");
    dispose_args_bundle(args_bundle);
    
    printf("bye\n");
    
    return 0;
}
