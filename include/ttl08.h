#ifndef LS08_H
#define LS08_H

#include "types.h"

struct ls08 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ls08 *ls08_create(void);
void ls08_free(struct ls08 *c);
void ls08_tick(struct ls08 *c);

#endif
