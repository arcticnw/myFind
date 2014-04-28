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
		"[options]:\n"
		"-follow\n"
		"  will resolve symlinks and will dive into direcotry symlinks\n"
		"-nofollow\n"
		"  won't resolve symlinks and won't dive into directory symlinks (default)\n"
		"-ignore-hidden\n"
		"  will ignore hidden files (default)\n"
		"-noignore-hidden\n"
		"  won't ignore hidden files\n"
		"\n"
		"[expression]:\n"
		"name filename\n"
		"  will match files to filename pattern\n"
		"iname filename\n"
		"  will match files to filename pattern, case insensitive\n"
		"true"
		"false"
		"\n"
		"[expression] and [expression]\n"
		"  both preceding and following operands have to be true\n"
		"  'and' operator has tighter associativity than 'or' operator\n"
		"[expression] or [expression]\n"
		"  either preceding or succeding operand has to be true\n"
		"( [expression] )\n"
		"  expressions contained within are grouped for the use of other operands\n"
		"not [expression]\n"
		"! [expression]\n"
		"  following expression will be negated\n"
		"\n"
		"print\n"
		"  local path to the matching files will be printed to standard output\n"
		"exec executable [arguments to executable | %s] ;\n"
		"  when file is matched an _executable_ will be ran with specified arguments\n"
		"  all '{}' arguments will be replaced with the name (and local path) of the matched file\n"
		"  list of parameters has to be terminated with ';'\n"
		"\n",
		name, EXEC_REPLACE_TOKEN);
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
