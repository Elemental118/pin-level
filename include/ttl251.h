#ifndef TTL251_H
#define TTL251_H

#include "types.h"

struct ttl251 {
	const bool *d[8];
	bool       *y;
	bool       *w;
	const bool *s[3];
	const bool *oe;
};

struct ttl251 *ttl251_create(void);
void ttl251_free(struct ttl251 *c);
void ttl251_tick(struct ttl251 *c);

#endif
