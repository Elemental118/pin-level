#ifndef TTL32_H
#define TTL32_H

#include "types.h"

struct ttl32 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ttl32 *ttl32_create(void);
void ttl32_free(struct ttl32 *c);
void ttl32_tick(struct ttl32 *c);

#endif
