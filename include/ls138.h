#ifndef LS138_H
#define LS138_H

#include "types.h"

struct ls138 {
	const bool *s[3];
	const bool *g1;
	const bool *g2[2];
	bool       *y[8];
};

struct ls138 *ls138_create(void);
void ls138_free(struct ls138 *c);
void ls138_tick(struct ls138 *c);

#endif
