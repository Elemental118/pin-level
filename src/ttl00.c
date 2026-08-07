#include <stdlib.h>

#include "ttl00.h"

struct ttl00 *ttl00_create(void)
{
	struct ttl00 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl00_free(struct ttl00 *c)
{
	free(c);
}

void ttl00_tick(struct ttl00 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = !(*c->a[i] & *c->b[i]);
	}
}
