#include <stdlib.h>

#include "ttl153.h"

struct ttl153 *ttl153_create(void)
{
	struct ttl153 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl153_free(struct ttl153 *c)
{
	free(c);
}

void ttl153_tick(struct ttl153 *c)
{
	for (int i = 0; i < 2; i++) {
		if (*c->g[i]) {
			*c->y[i] = false;
		} else {
			*c->y[i] = *c->d[i][(u8)*c->s[0] | ((u8)*c->s[1] << 1)];
		}
	}
}
