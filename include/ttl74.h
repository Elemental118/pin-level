#ifndef TTL74_H
#define TTL74_H

#include "types.h"

struct ttl74 {
	const bool *pr[2];
	const bool *clr[2];
	const bool *clk[2];
	const bool *d[2];
	bool       *q[2];
	bool       *nq[2];

	struct ttl74_priv *priv;
};

struct ttl74 *ttl74_create(void);
void ttl74_free(struct ttl74 *c);
void ttl74_tick(struct ttl74 *c);

#endif
