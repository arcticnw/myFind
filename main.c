#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "action.h"
#include "parser.h"
#include "crawler.h"

void
usage(const char *name) {
	printf("Usage: %s path [options] [expression [expression [..]]]\n\n",
	    name);

	/* options */
	printf("[options]:\n\n");

	/* follow, no follow */
	printf("%s\n    %s\n", "-follow",
	    "will resolve symlinks and will dive into directory symlinks");
	printf("%s\n    %s\n    %s\n", "-nofollow",
	    "won't resolve symlinks and won't dive into directory symlinks",
	    "default");

	/* ignore hidden, no ignore hidden */
	printf("%s\n    %s\n    %s\n", "-ignorehidden",
	    "will ignore hidden files",
	    "default");
	printf("%s\n    %s\n", "-noignorehidden",
	    "won't ignore hidden files");

	/* min depth, max depth */
	printf("%s\n    %s\n", "-mindepth n",
	    "will ignore hidden files");
	printf("%s\n    %s\n", "-maxdepth n",
	    "won't ignore hidden files");


	/* expressions */
	printf("\n\n[expression]:\n(without preceding dash symbol)\n\n");

	/* name, iname */
	printf("%s\n    %s\n", "name filename",
	    "will match files with filename pattern");
	printf("%s\n    %s\n    %s\n", "iname filename",
	    "will match files with filename pattern",
	    "case insensitive");
	printf("\n");

	/* amin, atime, anewer */
	printf("%s\n    %s\n", "amin n",
		"will match files accessed n minutes ago");
	printf("%s\n    %s\n", "atime n",
		"will match files accessed n days ago");
	printf("%s\n    %s\n", "anewer file",
		"will match files accessed more recently than file");
	printf("\n");

	/* cmin, ctime, cnewer */
	printf("%s\n    %s\n", "cmin n",
		"will match files created n minutes ago");
	printf("%s\n    %s\n", "ctime n",
		"will match files created n days ago");
	printf("%s\n    %s\n", "cnewer file",
		"will match files created more recently than file");
	printf("\n");

	/* mmin, mtime, mnewer */
	printf("%s\n    %s\n", "mmin n",
		"will match files modified n minutes ago");
	printf("%s\n    %s\n", "mtime n",
		"will match files modified n days ago");
	printf("%s\n    %s\n", "mnewer file",
		"will match files modified more recently than file");
	printf("\n");

	printf("%s\n    %s\n", "empty",
		"will match empty regular files and empty directories");
	printf("%s\n    %s\n", "size n",
		"will compare the file size with provided number");
	printf("\n");

	printf("%s\n    %s\n", "gid n",
		"will match file group id with provided number");
	printf("%s\n    %s\n", "group name",
		"will match file group with provided name");
	printf("%s\n    %s\n", "uid n",
		"will match file user id with provided number");
	printf("%s\n    %s\n", "user name",
		"will match file user with provided name");
	printf("\n");


	/* about numeric arguments */
	printf("\nNumeric arguments can be specified as:\n"
	    "+n   for expressing 'greather than' n\n"
	    " n   for expressing 'equal to' n\n"
	    "-n   for expressing 'less than' n\n\n\n");

	/* boolean operations */
	printf("%s\n    %s\n", "true",
	    "always passes");
	printf("%s\n    %s\n", "false",
	    "never passes");
	printf("\n");
	printf("%s\n    %s\n    %s\n", "[expression] and [expression]",
	    "both preceding and following operands have to be true",
	    "'and' operator has tighter associativity than 'or' operator");
	printf("%s\n    %s\n", "[expression] or [expression]",
	    "either preceding or succeding operand has to be true");
	printf("%s\n    %s\n", "([expression])",
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

int
main(int argc, char **argv) {
	args_bundle_t *args_bundle;

	if (argc < 2) {
		usage(argv[0]);
		return (0);
	}

	args_bundle = parse_arguments(argc, argv);

	crawl(args_bundle);

	dispose_args_bundle(args_bundle);

	return (0);
}
