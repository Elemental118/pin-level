#include <stdlib.h>

#include "ls259.h"

struct ls259_priv {
	bool q_prev[8];
};

struct ls259 *ls259_create(void)
{
	struct ls259 *c = calloc(9, sizeof(*c));
	return c;
}

void ls259_free(struct ls259 *c)
{
	free(c);
}

void ls259_tick(struct ls259 *c)
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
