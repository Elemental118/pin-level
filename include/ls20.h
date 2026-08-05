#ifndef LS20_H
#define LS20_H

#include "types.h"

struct ls20 {
	const bool *a[2];
	const bool *b[2];
	const bool *c[2];
	const bool *d[2];
	bool       *y[2];
};

struct ls20 *ls20_create(void);
void ls20_free(struct ls20 *c);
void ls20_tick(struct ls20 *c);

#endif
