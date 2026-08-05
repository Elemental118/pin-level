#include <stdlib.h>

#include "ttl08.h"

struct ttl08 *ttl08_create(void)
{
	struct ttl08 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl08_free(struct ttl08 *c)
{
	free(c);
}

void ttl08_tick(struct ttl08 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = *c->a[i] & *c->b[i];
	}
}
