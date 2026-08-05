#ifndef TTL257_H
#define TTL257_H

#include "types.h"

struct ttl257 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
	const bool *oe;
	const bool *s;
};

struct ttl257 *ttl257_create(void);
void ttl257_free(struct ttl257 *c);
void ttl257_tick(struct ttl257 *c);

#endif
