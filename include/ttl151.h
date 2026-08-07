#ifndef TTL151_H
#define TTL151_H

#include "types.h"

struct ttl151 {
	const bool *d[8];
	bool       *y;
	bool       *w;
	const bool *s[3];
	const bool *g;
};

struct ttl151 *ttl151_create(void);
void ttl151_free(struct ttl151 *c);
void ttl151_tick(struct ttl151 *c);

#endif
