#include <stdlib.h>

#include "ttl86.h"

struct ttl86 *ttl86_create(void)
{
	struct ttl86 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl86_free(struct ttl86 *c)
{
	free(c);
}

void ttl86_tick(struct ttl86 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = *c->a[i] ^ *c->b[i];
	}
}
