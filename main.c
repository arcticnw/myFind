#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "action.h"
#include "parser.h"
#include "crawler.h"

void
help(const char *name) {
	printf("Usage: %s path [options] [expression [expression [..]]]\n\n",
	    name);

	/* options */
	printf("[options]:\n\n");

	printf("--help\n    will show this screen, won't perform any search\n");

	/* follow, no follow */
	printf("-follow\n    will resolve symlinks and will dive into "
	    "directory symlinks\n");
	printf("-nofollow\n    won't resolve symlinks and won't dive into "
	    "directory symlinks\n    default");

	/* ignore hidden, no ignore hidden */
	printf("-ignorehidden\n    will ignore hidden files\n    default\n");
	printf("-noignorehidden\n    won't ignore hidden files\n");

	/* min depth, max depth */
	printf("-mindepth n\n    minimal level in the directory tree before "
	    "the search will be performed\n");
	printf("-maxdepth n\n    maximal level in the directory tree before "
	    "the search will no longer search\n");


	/* expressions */
	printf("\n\n[expression]:\n(without preceding dash symbol)\n\n");

	/* name, iname */
	printf("name filename\n    will match files with filename pattern\n");
	printf("iname filename\n    will match files with case insensitive"
	    "filename pattern\n");
	printf("\n");

	/* amin, atime, anewer */
	printf("amin n\n    will match files accessed n minutes ago\n");
	printf("atime n\n    will match files accessed n days ago\n");
	printf("anewer file\n    will match files accessed more recently "
	    "than file\n");
	printf("\n");

	/* cmin, ctime, cnewer */
	printf("cmin n\n    will match files created n minutes ago\n");
	printf("ctime n\n    will match files created n days ago\n");
	printf("cnewer file\n    will match files created more recently "
	    "than file\n");
	printf("\n");

	/* mmin, mtime, mnewer */
	printf("mmin n\n    will match files modified n minutes ago\n");
	printf("mtime n\n    will match files modified n days ago\n");
	printf("mnewer file\n    will match files modified more recently "
	    "than file\n");
	printf("\n");

	printf("empty\n    will match empty regular files and empty "
	    "directories\n");
	printf("size n\n    will compare the file size with provided number\n");
	printf("\n");

	printf("gid n\n    will match file group id with provided number\n");
	printf("group name\n    will match file group with provided name\n");
	printf("uid n\n    will match file user id with provided number\n");
	printf("user name\n    will match file user with provided name\n");
	printf("\n");


	/* about numeric arguments */
	printf("\nNumeric arguments can be specified as:\n"
	    "+n   for expressing 'greather than' n\n"
	    " n   for expressing 'equal to' n\n"
	    "-n   for expressing 'less than' n\n\n\n");

	/* boolean operations */
	printf("true\n    always passes\n");
	printf("false\n    never passes\n");
	printf("\n");
	printf("[expression] and [expression]\n    both preceding and "
	    "following operands have to be true\n    'and' operator has "
	    "tighter associativity than 'or' operator\n");
	printf("[expression] or [expression]\n    either preceding or "
	    "succeding operand has to be true\n");
	printf("([expression])\n    expressions contained within are grouped "
	    "for the use of other operands\n");
	printf("not [expression]\n! [expression]\n    following expression "
	    "will be negated\n");

	/* actions */
	printf("\n");
	printf("print\n    local path to the matching files will be printed "
	    "to standard output\n");
	printf("exec executable [arguments to executable | %s] ;\n"
	    "    when file is matched an _executable_ will be ran with "
	    "specified arguments\n"
	    "    all '%s' arguments will be replaced with the name "
	    "(and local path) of the matched file\n"
	    "    list of parameters has to be terminated with ';'",
	    EXEC_REPLACE_TOKEN, EXEC_REPLACE_TOKEN);

	printf("\n\n");
}

void
usage(const char *name) {
	printf("Usage: %s path [options] [expression [expression [..]]]\n\n",
	    name);

	printf("options are: -follow, -nofollow, -ignorehidden, "
	    "-noignorehidden, -mindepth n, -maxdepth n, --help\n\n");

	printf("expression may be one of following (without preceding dash "
	    "symbol):\n");
	printf("tests: name filename, iname filename, filename pattern, "
	    "amin n, atime n, anewer file, cmin n, ctime n, cnewer file, "
	    "mmin n, mtime n, mnewer file, empty, size n, gid n, group name, "
	    "uid n, user name\n");
	printf("operators: true, false, expr and expr, expr or expr, ( expr ), "
	    "not [expr], ! [expr]\n");

	printf("actions: print, exec executable [args | %s] ;\n",
	    EXEC_REPLACE_TOKEN);

	printf("\n\n");
}

int
main(int argc, char **argv) {
	args_bundle_t *args_bundle;

	if (argc < 2) {
		usage(argv[0]);
		return (0);
	}

	args_bundle = parse_arguments(argc, argv);

	if (args_bundle->show_help) {
		help(argv[0]);
	} else {
		crawl(args_bundle);
	}

	dispose_args_bundle(args_bundle);

	return (0);
}
