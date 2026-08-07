#ifndef TTL02_H
#define TTL02_H

#include "types.h"

struct ttl02 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ttl02 *ttl02_create(void);
void ttl02_free(struct ttl02 *c);
void ttl02_tick(struct ttl02 *c);

#endif
