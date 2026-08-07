#ifndef TTL00_H
#define TTL00_H

#include "types.h"

struct ttl00 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ttl00 *ttl00_create(void);
void ttl00_free(struct ttl00 *c);
void ttl00_tick(struct ttl00 *c);

#endif
