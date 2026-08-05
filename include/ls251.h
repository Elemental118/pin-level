#ifndef LS251_H
#define LS251_H

#include "types.h"

struct ls251 {
	const bool *d[8];
	bool       *y;
	bool       *w;
	const bool *s[3];
	const bool *oe;
};

struct ls251 *ls251_create(void);
void ls251_free(struct ls251 *c);
void ls251_tick(struct ls251 *c);

#endif
