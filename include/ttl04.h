#ifndef TTL04_H
#define TTL04_H

#include "types.h"

struct ttl04 {
	const bool *a[6];
	bool       *y[6];
};

struct ttl04 *ttl04_create(void);
void ttl04_free(struct ttl04 *c);
void ttl04_tick(struct ttl04 *c);

#endif
