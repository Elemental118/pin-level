#include <stdlib.h>

#include "ls32.h"

struct ls32 *ls32_create(void)
{
	struct ls32 *c = calloc(1, sizeof(*c));
	return c;
}

void ls32_free(struct ls32 *c)
{
	free(c);
}

void ls32_tick(struct ls32 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = *c->a[i] | *c->b[i];
	}
}
