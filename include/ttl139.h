#ifndef TTL139_H
#define TTL139_H

#include "types.h"

struct ttl139 {
	const bool *s[2][2];
	const bool *g[2];
	bool       *y[2][4];
};

struct ttl139 *ttl139_create(void);
void ttl139_free(struct ttl139 *c);
void ttl139_tick(struct ttl139 *c);

#endif
