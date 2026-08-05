#include <stdlib.h>

#include "ls138.h"

struct ls138 *ls138_create(void)
{
	struct ls138 *c = calloc(1, sizeof(*c));
	return c;
}

void ls138_free(struct ls138 *c)
{
	free(c);
}

void ls138_tick(struct ls138 *c)
{
	for (int i = 0; i < 8; i++) {
		*c->y[i] = true;
	}
	if (!(*c->g1 && !*c->g2[0] && !*c->g2[1])) {
		return;
	}
	*c->y[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)] = false;
}
