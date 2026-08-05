#include <stdlib.h>

#include "ls20.h"

struct ls20 *ls20_create(void)
{
	struct ls20 *c = calloc(1, sizeof(*c));
	return c;
}

void ls20_free(struct ls20 *c)
{
	free(c);
}

void ls20_tick(struct ls20 *c)
{
	for (int i = 0; i < 2; i++) {
		*c->y[i] = !(*c->a[i] & *c->b[i] & *c->c[i] & *c->d[i]);
	}
}
