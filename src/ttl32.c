#include <stdlib.h>

#include "ttl32.h"

struct ttl32 *ttl32_create(void)
{
	struct ttl32 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl32_free(struct ttl32 *c)
{
	free(c);
}

void ttl32_tick(struct ttl32 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = *c->a[i] | *c->b[i];
	}
}
