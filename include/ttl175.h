#ifndef TTL175_H
#define TTL175_H

#include "types.h"

struct ttl175 {
	bool       *q[4];
	bool       *nq[4];
	const bool *d[4];
	const bool *clk;
	const bool *clr;

	struct ttl175_priv *priv;
};

struct ttl175 *ttl175_create(void);
void ttl175_free(struct ttl175 *c);
void ttl175_tick(struct ttl175 *c);

#endif
