#ifndef TTL138_H
#define TTL138_H

#include "types.h"

struct ttl138 {
	const bool *s[3];
	const bool *g1;
	const bool *g2[2];
	bool       *y[8];
};

struct ttl138 *ttl138_create(void);
void ttl138_free(struct ttl138 *c);
void ttl138_tick(struct ttl138 *c);

#endif
