#include <stdlib.h>

#include "ls251.h"

struct ls251 *ls251_create(void)
{
	struct ls251 *c = calloc(1, sizeof(*c));
	return c;
}

void ls251_free(struct ls251 *c)
{
	free(c);
}

void ls251_tick(struct ls251 *c)
{
	if (*c->s) {
		return;
	}
	*c->y =  *c->d[(u8)*c->sel[0] | ((u8)*c->sel[1] << 1) | ((u8)*c->sel[2] << 2)];
	*c->w = !*c->y;
}
