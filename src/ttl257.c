#include <stdlib.h>

#include "ttl257.h"

struct ttl257 *ttl257_create(void)
{
	struct ttl257 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl257_free(struct ttl257 *c)
{
	free(c);
}

void ttl257_tick(struct ttl257 *c)
{
	if (!*c->oe) {
		for (int i = 0; i < 4; i++) {
			*c->y[i] = *c->s ? *c->b[i] : *c->a[i];
		}
	}
}
