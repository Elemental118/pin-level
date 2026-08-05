#ifndef TTL51_H
#define TTL51_H

#include "types.h"

struct ttl51 {
	const bool *a1[3];
	const bool *b1[3];
	const bool *a2[2];
	const bool *b2[2];
	bool       *y[2];
};

struct ttl51 *ttl51_create(void);
void ttl51_free(struct ttl51 *c);
void ttl51_tick(struct ttl51 *c);

#endif
