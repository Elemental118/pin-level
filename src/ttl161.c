#include <stdlib.h>

#include "ttl161.h"

#define CLK_RISING(c) ((bool)(!(c)->priv->clk_prev && *(c)->clk))

struct ttl161_priv {
	bool clk_prev;
	u8 num_prev;
};

struct ttl161 *ttl161_create(void)
{
	struct ttl161 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));
	return c;
}

void ttl161_free(struct ttl161 *c)
{
	free(c->priv);
	free(c);
}

void ttl161_tick(struct ttl161 *c)
{
	if (!*c->clr) {
		c->priv->num_prev = 0;
		for (int i = 0; i < 4; i++) {
			*c->q[i] = false;
		}
	} else if (CLK_RISING(c) && !*c->load) {
		c->priv->num_prev = 0;
		for (int i = 0; i < 4; i++) {
			c->priv->num_prev |= ((u8)*c->d[i] << i);
			*c->q[i] = *c->d[i];
		}
	} else if (CLK_RISING(c) && *c->enp && *c->ent) {
		c->priv->num_prev = (c->priv->num_prev + 1) & 0b1111;
		for (int i = 0; i < 4; i++) {
			*c->q[i] = (c->priv->num_prev >> i) & 1;
		}
	}

	*c->rco = *c->ent && (c->priv->num_prev == 0b1111);
	c->priv->clk_prev = *c->clk;
}
