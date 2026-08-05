#include <stdlib.h>

#include "ls51.h"

struct ls51 *ls51_create(void)
{
	struct ls51 *c = calloc(1, sizeof(*c));
	return c;
}

void ls51_free(struct ls51 *c)
{
	free(c);
}

void ls51_tick(struct ls51 *c)
{
	*c->y[0] = !((*c->a1[0] & *c->a1[1] & *c->a1[2]) | (*c->b1[0] & *c->b1[1] & *c->b1[2]));
	*c->y[1] = !((*c->a2[0] & *c->a2[1]) | (*c->b2[0] & *c->b2[1]));
}
