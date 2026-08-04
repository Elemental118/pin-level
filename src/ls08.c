#include <stdlib.h>

#include "ls08.h"

struct ls08 *ls08_create(void)
{
	struct ls08 *c = calloc(1, sizeof(*c));
	return c;
}

void ls08_free(struct ls08 *c)
{
	free(c);
}

void ls08_tick(struct ls08 *c)
{
	for (int i = 0; i < 4; i++) {
		*c->y[i] = *c->a[i] & *c->b[i];
	}
}
