#include <stdlib.h>

#include "ls139.h"

struct ls139 *ls139_create(void)
{
	struct ls139 *c = calloc(1, sizeof(*c));
	return c;
}

void ls139_free(struct ls139 *c)
{
	free(c);
}

void ls139_tick(struct ls139 *c)
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			*c->y[i][j] = true;
		}
		if (*c->g[i]) {
			continue;
		}
		*c->y[i][(u8)*c->s[i][0] | ((u8)*c->s[i][1] << 1)] = false;
	}
}
