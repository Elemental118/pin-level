#ifndef TTL259_H
#define TTL259_H

#include "types.h"

struct ttl259 {
	bool       *q[8];
	const bool *d;
	const bool *clr;
	const bool *we;
	const bool *s[3];

	struct ttl259_priv *priv;
};

struct ttl259 *ttl259_create(void);
void ttl259_free(struct ttl259 *c);
void ttl259_tick(struct ttl259 *c);

#endif
