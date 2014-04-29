#include <stdio.h>

#include "common.h"
#include "parser.h"
#include "checker.h"
#include "action.h"
#include "debug.h"

void dumpArgsBundle(args_bundle_t *bundle) {
	printf("# DUMP #\n");

	printf("path: %s\n", bundle->path);

	printf("flags: ");
	dumpFlags(bundle);
	printf("\n");

	printf("condition: ");
	dumpCondition(bundle->condition);
	printf("\n"); 

	printf("action: ");
	dumpAction(bundle->action);
	printf("\n"); 

	printf("# END #\n");
}

void dumpFlags(args_bundle_t *bundle) {
	printf("; followLinks: ");
	if (bundle->follow_links) { printf("yes"); } else { printf("no"); }
	printf("; ignoreHidden: ");
	if (bundle->ignore_hidden) { printf("yes"); } else { printf("no"); }
}

void dumpData(data_t data, content_t content) {
	if (content == STRING) {
		printf("%s", data.string_data);
	}
	else if (content == LONG) {
		printf("%ld", data.long_data);
	}
	else if (content == CONDITION) {
		dumpCondition(data.condition_data);
	}
	else if (content == NONE) {
		printf("-");
	}
	else {
		printf("?");
	}
}

void dumpCondition(condition_t * c) {
	if (!c) {
		printf("#NULL#");
		return;
	}

	if (c->do_check == check_or || c->do_check == check_and) {
		printf("[");
	}
	else if (c->do_check == check_not) {
		printf("!(");
	}
	else if (c->do_check == check_true) {
		printf("<True> /*");
	}
	else if (c->do_check == check_false) {
		printf("<False> /*");
	}
	else {
		printf("<Fct> (");
	}

	dumpData(c->data1, c->data1_content);

	if (c->do_check == check_or) {
		printf("] or [");
	}
	else if (c->do_check == check_and) {
		printf("] and [");
	}
	else if (c->do_check == check_not) {
		printf(" /*");
	}
	else {
		printf(", ");
	}

	dumpData(c->data2, c->data2_content);

	if (c->do_check == check_or || c->do_check == check_and) {
		printf("]");
	}
	else if (c->do_check == check_true) {
		printf("*/");
	}
	else if (c->do_check == check_false) {
		printf("*/");
	}
	else if (c->do_check == check_not) {
		printf("*/)");
	}
	else {
		printf(")");
	}
}

void dumpAction(action_t * action) {
	int i;

	if (action->do_action) {
		printf("<Fct[%d]> (", action->param_count);
	}
	else {
		printf("<Null[%d]> (", action->param_count);
	}

	if (!action->params) {
	    printf("#NULL#");
	}
	else {
		for(i = 0; i < action->param_count; i++) {
		    printf("%s, ", action->params[i]);
		}
	}
	printf(") ");
	if (action->next) {
	    dumpAction(action->next);
	}
}

