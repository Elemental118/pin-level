#include <stdlib.h>

#include "ttl251.h"

struct ttl251 *ttl251_create(void)
{
	struct ttl251 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl251_free(struct ttl251 *c)
{
	free(c);
}

void ttl251_tick(struct ttl251 *c)
{
	if (*c->oe) {
		return;
	}
	*c->y =  *c->d[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)];
	*c->w = !*c->y;
}
