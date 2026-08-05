#include <stdlib.h>

#include "ttl138.h"

struct ttl138 *ttl138_create(void)
{
	struct ttl138 *c = calloc(1, sizeof(*c));
	return c;
}

void ttl138_free(struct ttl138 *c)
{
	free(c);
}

void ttl138_tick(struct ttl138 *c)
{
	for (int i = 0; i < 8; i++) {
		*c->y[i] = true;
	}
	if (!(*c->g1 && !*c->g2[0] && !*c->g2[1])) {
		return;
	}
	*c->y[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)] = false;
}
