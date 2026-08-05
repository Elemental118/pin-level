#ifndef TTL08_H
#define TTL08_H

#include "types.h"

struct ttl08 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ttl08 *ttl08_create(void);
void ttl08_free(struct ttl08 *c);
void ttl08_tick(struct ttl08 *c);

#endif
