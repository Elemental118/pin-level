#ifndef TTL174_H
#define TTL174_H

#include "types.h"

struct ttl174 {
	bool       *q[6];
	const bool *d[6];
	const bool *clk;
	const bool *clr;

	struct ttl174_priv *priv;
};

struct ttl174 *ttl174_create(void);
void ttl174_free(struct ttl174 *c);
void ttl174_tick(struct ttl174 *c);

#endif
