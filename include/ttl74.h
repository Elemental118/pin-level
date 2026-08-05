#ifndef LS74_H
#define LS74_H

#include "types.h"

struct ls74 {
	const bool *pr[2];
	const bool *clr[2];
	const bool *clk[2];
	const bool *d[2];
	bool       *q[2];
	bool       *nq[2];

	struct ls74_priv *priv;
};

struct ls74 *ls74_create(void);
void ls74_free(struct ls74 *c);
void ls74_tick(struct ls74 *c);

#endif
