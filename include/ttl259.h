#ifndef LS259_H
#define LS259_H

#include "types.h"

struct ls259 {
	bool       *q[8];
	const bool *d;
	const bool *clr;
	const bool *we;
	const bool *s[3];

	struct ls259_priv *priv;
};

struct ls259 *ls259_create(void);
void ls259_free(struct ls259 *c);
void ls259_tick(struct ls259 *c);

#endif
