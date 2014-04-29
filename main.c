#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "action.h"
#include "parser.h"
#include "crawler.h"
#include "debug.h"

void usage(const char *name)
{ /*                                                                            v*/
	printf("Usage: %s path [options] [expression [expression [..]]]\n\n", 
	    name);
	
	/* options */
	printf("[options]:\n");
	printf("%s\n    %s\n", "-follow",
	    "will resolve symlinks and will dive into directory symlinks");
	printf("%s\n    %s\n    %s\n", "-nofollow", 
	    "won't resolve symlinks and won't dive into directory symlinks",
	    "default");
	printf("%s\n    %s\n    %s\n", "-ignore-hidden", 
	    "will ignore hidden files", 
	    "default");
	printf("%s\n    %s\n", "-noignore-hidden",
	    "won't ignore hidden files");
	
	/* expressions */
	printf("\n[expression]:\n");
	printf("%s\n    %s\n", "name filename", 
	    "will match files to filename pattern");
	printf("%s\n    %s\n    %s\n", "iname filename", 
	    "will match files to filename pattern",
	    "case insensitive");
	printf("%s\n    %s\n", "true", 
	    "always passes");
	printf("%s\n    %s\n", "false", 
	    "never passes");
	printf("%s\n    %s\n    %s\n", "[expression] and [expression]", 
	    "both preceding and following operands have to be true",
	    "'and' operator has tighter associativity than 'or' operator");
	printf("%s\n    %s\n", "[expression] or [expression]", 
	    "either preceding or succeding operand has to be true");
	printf("%s\n    %s\n", "( [expression] )", 
	    "expressions contained within are grouped for the use of other "
	    "operands");
	printf("%s\n%s\n    %s\n", "not [expression]", "! [expression]", 
	    "following expression will be negated");
	
	/* actions */
	printf("\n");
	printf("%s\n    %s\n", "print", 
	    "local path to the matching files will be printed to"
	    "standard output");
	printf(
	    "exec executable [arguments to executable | %s] ;\n"
	    "    when file is matched an _executable_ will be ran with "
	    "specified arguments\n"
	    "    all '%s' arguments will be replaced with the name "
	    "(and local path) of the matched file\n"
	    "    list of parameters has to be terminated with ';'",
	    EXEC_REPLACE_TOKEN, EXEC_REPLACE_TOKEN);
	    
	printf("\n\n");
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
