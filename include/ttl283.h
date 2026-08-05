#ifndef TTL283_H
#define TTL283_H

#include "types.h"

struct ttl283 {
	const bool *a[4];
	const bool *b[4];
	bool       *y[4];
	const bool *cin;
	bool       *cout;
};

struct ttl283 *ttl283_create(void);
void ttl283_free(struct ttl283 *c);
void ttl283_tick(struct ttl283 *c);

#endif
