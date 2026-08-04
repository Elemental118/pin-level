#include <stdlib.h>

#include "ls257.h"

struct ls257 *ls257_create(void)
{
	struct ls257 *c = calloc(1, sizeof(*c));
	return c;
}

void ls257_free(struct ls257 *c)
{
	free(c);
}

void ls257_tick(struct ls257 *c)
{
	if (!*c->oe) {
		for (int i = 0; i < 4; i++) {
			*c->y[i] = *c->sel ? *c->b[i] : *c->a[i];
		}
	}
}
