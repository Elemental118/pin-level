#ifndef LS257_H
#define LS257_H

#include "types.h"

struct ls257 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
	const bool *oe;
	const bool *sel;
};

struct ls257 *ls257_create(void);
void ls257_free(struct ls257 *c);
void ls257_tick(struct ls257 *c);

#endif
