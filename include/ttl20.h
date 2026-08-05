#ifndef TTL20_H
#define TTL20_H

#include "types.h"

struct ttl20 {
	const bool *a[2];
	const bool *b[2];
	const bool *c[2];
	const bool *d[2];
	bool       *y[2];
};

struct ttl20 *ttl20_create(void);
void ttl20_free(struct ttl20 *c);
void ttl20_tick(struct ttl20 *c);

#endif
