#ifndef _DEBUG_H_
#define	_DEBUG_H_

#include "common.h"

void
dumpArgsBundle(args_bundle_t *bundle);

void
dumpFlags(args_bundle_t *bundle);

void
dumpData(data_t data, content_t content);

void
dumpCondition(condition_t *condition);

void
dumpAction(action_t *action);

#endif