#ifndef TTL86_H
#define TTL86_H

#include "types.h"

struct ttl86 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
};

struct ttl86 *ttl86_create(void);
void ttl86_free(struct ttl86 *c);
void ttl86_tick(struct ttl86 *c);

#endif
