#ifndef TTL11_H
#define TTL11_H

#include "types.h"

struct ttl11 {
	const bool *a[3];
	const bool *b[3];
	const bool *c[3];
	bool       *y[3];
};

struct ttl11 *ttl11_create(void);
void ttl11_free(struct ttl11 *c);
void ttl11_tick(struct ttl11 *c);

#endif
