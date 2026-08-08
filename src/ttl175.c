#include <stdlib.h>

#include "ttl175.h"

#define CLK_RISING(c) ((bool)(!(c)->priv->clk_prev && *(c)->clk))

struct ttl175_priv {
	bool q_prev[6];
	bool clk_prev;
};

struct ttl175 *ttl175_create(void)
{
	struct ttl175 *c = calloc(7, sizeof(*c));
	return c;
}

void ttl175_free(struct ttl175 *c)
{
	free(c);
}

void ttl175_tick(struct ttl175 *c)
{
	if (!*c->clr) {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = false;
		}
	} else if (!CLK_RISING(c)) {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = c->priv->q_prev[i];
		}
	} else {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = *c->d[i];
		}
	}

	for (int i = 0; i < 4; i++) {
		c->priv->q_prev[i] = *c->q[i];
		*c->nq[i] = !*c->q[i];
	}
	c->priv->clk_prev = *c->clk;
}
