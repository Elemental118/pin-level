#ifndef LS283_H
#define LS283_H

#include "types.h"

struct ls283 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
	const bool *cin;
	bool       *cout;
};

struct ls283 *ls283_create(void);
void ls283_free(struct ls283 *c);
void ls283_tick(struct ls283 *c);

#endif
