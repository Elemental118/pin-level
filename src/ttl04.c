#include <stdlib.h>

#include "ttl04.h"

struct ttl04 *ttl04_create(void)
{
	struct ttl04 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl04_free(struct ttl04 *c)
{
	free(c);
}

void ttl04_tick(struct ttl04 *c)
{
	for (int i = 0; i < 6; i++) {
		*c->y[i] = !*c->a[i];
	}
}
