#include <stdlib.h>

#include "ttl11.h"

struct ttl11 *ttl11_create(void)
{
	struct ttl11 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl11_free(struct ttl11 *c)
{
	free(c);
}

void ttl11_tick(struct ttl11 *c)
{
	for (int i = 0; i < 3; i++) {
		*c->y[i] = *c->a[i] & *c->b[i] & *c->c[i];
	}
}
