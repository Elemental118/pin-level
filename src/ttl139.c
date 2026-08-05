#include <stdlib.h>

#include "ttl139.h"

struct ttl139 *ttl139_create(void)
{
	struct ttl139 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl139_free(struct ttl139 *c)
{
	free(c);
}

void ttl139_tick(struct ttl139 *c)
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
