#ifndef TTL161_H
#define TTL161_H

#include "types.h"

struct ttl161 {
	const bool *d[4];
	bool       *q[4];
	const bool *enp;
	const bool *ent;
	const bool *clk;
	const bool *load;
	const bool *clr;
	bool       *rco;

	struct ttl161_priv *priv;
};

struct ttl161 *ttl161_create(void);
void ttl161_free(struct ttl161 *c);
void ttl161_tick(struct ttl161 *c);

#endif
