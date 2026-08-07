#include <stdlib.h>

#include "ttl151.h"

struct ttl151 *ttl151_create(void)
{
	struct ttl151 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl151_free(struct ttl151 *c)
{
	free(c);
}

void ttl151_tick(struct ttl151 *c)
{
	if (*c->g) {
		*c->y = false;
	} else {
		*c->y =  *c->d[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)];
	}
	*c->w = !*c->y;
}
