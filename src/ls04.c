#include <stdlib.h>

#include "ls04.h"

struct ls04 *ls04_create(void)
{
	struct ls04 *c = calloc(1, sizeof(*c));
	return c;
}

void ls04_free(struct ls04 *c)
{
	free(c);
}

void ls04_tick(struct ls04 *c)
{
	for (int i = 0; i < 6; i++) {
		*c->y[i] = !*c->a[i];
	}
}
