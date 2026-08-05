#include <stdlib.h>

#include "ttl20.h"

struct ttl20 *ttl20_create(void)
{
	struct ttl20 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl20_free(struct ttl20 *c)
{
	free(c);
}

void ttl20_tick(struct ttl20 *c)
{
	for (int i = 0; i < 2; i++) {
		*c->y[i] = !(*c->a[i] & *c->b[i] & *c->c[i] & *c->d[i]);
	}
}
