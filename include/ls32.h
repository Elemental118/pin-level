#ifndef LS32_H
#define LS32_H

#include "types.h"

struct ls32 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ls32 *ls32_create(void);
void ls32_free(struct ls32 *c);
void ls32_tick(struct ls32 *c);

#endif
