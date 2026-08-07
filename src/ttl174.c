#include <stdlib.h>

#include "ttl174.h"

#define CLK_RISING(c) ((bool)(!(c)->priv->clk_prev && *(c)->clk))

struct ttl174_priv {
	bool q_prev[6];
	bool clk_prev;
};

struct ttl174 *ttl174_create(void)
{
	struct ttl174 *c = calloc(7, sizeof(*c));
	return c;
}

void ttl174_free(struct ttl174 *c)
{
	free(c);
}

void ttl174_tick(struct ttl174 *c)
{
	if (!*c->clr) {
		for (int i = 0; i < 6; i++) {
			*c->q[i] = false;
		}
	} else if (!CLK_RISING(c)) {
		for (int i = 0; i < 6; i++) {
			*c->q[i] = c->priv->q_prev[i];
		}
	} else {
		for (int i = 0; i < 6; i++) {
			*c->q[i] = *c->d[i];
		}
	}

	for (int i = 0; i < 6; i++) {
		c->priv->q_prev[i] = *c->q[i];
	}
	c->priv->clk_prev = *c->clk;
}
