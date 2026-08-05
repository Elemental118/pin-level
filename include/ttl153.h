#ifndef TTL153_H
#define TTL153_H

#include "types.h"

struct ttl153 {
	const bool *d[2][4];
	const bool *s[2];
	const bool *g[2];
	bool       *y[2];
};

struct ttl153 *ttl153_create(void);
void ttl153_free(struct ttl153 *c);
void ttl153_tick(struct ttl153 *c);

#endif
