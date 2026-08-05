#include <stdlib.h>

#include "ls153.h"

struct ls153 *ls153_create(void)
{
	struct ls153 *c = calloc(1, sizeof(*c));
	return c;
}

void ls153_free(struct ls153 *c)
{
	free(c);
}

void ls153_tick(struct ls153 *c)
{
	for (int i = 0; i < 2; i++) {
		if (*c->g[i]) {
			*c->y[i] = false;
		} else {
			*c->y[i] = *c->d[i][(u8)*c->s[0] | ((u8)*c->s[1] << 1)];
		}
	}
}
