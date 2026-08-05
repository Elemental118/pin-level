#include <stdlib.h>

#include "ttl259.h"

struct ttl259_priv {
	bool q_prev[8];
};

struct ttl259 *ttl259_create(void)
{
	struct ttl259 *c = calloc(9, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));
	return c;
}

void ttl259_free(struct ttl259 *c)
{
	free(c->priv);
	free(c);
}

void ttl259_tick(struct ttl259 *c)
{
	if (*c->clr) {
		for (int i = 0; i < 8; i++) {
			*c->q[i] = c->priv->q_prev[i];
		}
	} else {
		for (int i = 0; i < 8; i++) {
			*c->q[i] = false;
		}
	}
	if (!*c->we) {
		*c->q[(u8)*c->s[0] | ((u8)*c->s[1] << 1) | ((u8)*c->s[2] << 2)] = *c->d;
	}

	for (int i = 0; i < 8; i++) {
		c->priv->q_prev[i] = *c->q[i];
	}
}
