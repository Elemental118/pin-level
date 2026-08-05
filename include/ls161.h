#ifndef LS161_H
#define LS161_H

#include "types.h"

struct ls161 {
	const bool *d[4];
	bool       *q[4];
	const bool *enp;
	const bool *ent;
	const bool *clk;
	const bool *load;
	const bool *clr;
	bool       *rco;

	struct ls161_priv *priv;
};

struct ls161 *ls161_create(void);
void ls161_free(struct ls161 *c);
void ls161_tick(struct ls161 *c);

#endif
