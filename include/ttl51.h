#ifndef LS51_H
#define LS51_H

#include "types.h"

struct ls51 {
	const bool *a1[3];
	const bool *b1[3];
	const bool *a2[2];
	const bool *b2[2];
	bool       *y[2];
};

struct ls51 *ls51_create(void);
void ls51_free(struct ls51 *c);
void ls51_tick(struct ls51 *c);

#endif
