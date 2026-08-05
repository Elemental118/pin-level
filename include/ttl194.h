#ifndef TTL194_H
#define TTL194_H

#include "types.h"

struct ttl194 {
	bool       *q[4];
	const bool *d[4];
	const bool *clk;
	const bool *clr;
	const bool *ser_l;
	const bool *ser_r;
	const bool *s[2];

	struct ttl194_priv *priv;
};

struct ttl194 *ttl194_create(void);
void ttl194_free(struct ttl194 *c);
void ttl194_tick(struct ttl194 *c);

#endif
