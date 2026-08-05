#include <stdlib.h>

#include "ttl51.h"

struct ttl51 *ttl51_create(void)
{
	struct ttl51 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl51_free(struct ttl51 *c)
{
	free(c);
}

void ttl51_tick(struct ttl51 *c)
{
	*c->y[0] = !((*c->a1[0] & *c->a1[1] & *c->a1[2]) | (*c->b1[0] & *c->b1[1] & *c->b1[2]));
	*c->y[1] = !((*c->a2[0] & *c->a2[1]) | (*c->b2[0] & *c->b2[1]));
}
