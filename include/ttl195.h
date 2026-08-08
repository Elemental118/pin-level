#ifndef TTL195_H
#define TTL195_H

#include "types.h"

struct ttl195 {
	bool       *q[4];
	bool       *nq;
	const bool *d[4];
	const bool *clk;
	const bool *clr;
	const bool *load;
	const bool *j;
	const bool *k;

	struct ttl195_priv *priv;
};

struct ttl195 *ttl195_create(void);
void ttl195_free(struct ttl195 *c);
void ttl195_tick(struct ttl195 *c);

#endif
