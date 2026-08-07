#include <stdlib.h>

#include "ttl02.h"

struct ttl02 *ttl02_create(void)
{
	struct ttl02 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl02_free(struct ttl02 *c)
{
	free(c);
}

void ttl02_tick(struct ttl02 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = !(*c->a[i] | *c->b[i]);
	}
}
