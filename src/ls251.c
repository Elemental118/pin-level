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
	if (*c->oe) {
		return;
	}
	*c->y =  *c->d[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)];
	*c->w = !*c->y;
}
