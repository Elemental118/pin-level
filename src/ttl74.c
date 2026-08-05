#include <stdlib.h>

#include "ttl74.h"

#define CLK_RISING(c, i) ((bool)(!(c)->priv->clk_prev[i] && *(c)->clk[i]))

struct ttl74_priv {
	bool q_prev[2];
	bool clk_prev[2];
};

struct ttl74 *ttl74_create(void)
{
	struct ttl74 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));
	return c;
}

void ttl74_free(struct ttl74 *c)
{
	free(c->priv);
	free(c);
}

void ttl74_tick(struct ttl74 *c)
{
	for (int i = 0; i < 2; i++) {
		if (!*c->pr[i] && *c->clr[i]) {
			*c->q[i]  = true;
			*c->nq[i] = false;
			c->priv->q_prev[i] = true;
		} else if (*c->pr[i] && !*c->clr[i]) {
			*c->q[i]  = false;
			*c->nq[i] = true;
			c->priv->q_prev[i] = false;
		} else if (*c->pr[i] && *c->clr[i] && CLK_RISING(c, i)) {
			*c->q[i]  = *c->d[i];
			*c->nq[i] = !*c->d[i];
			c->priv->q_prev[i] = *c->d[i];
		} else if (*c->pr[i] && *c->clr[i]) {
			*c->q[i]  = c->priv->q_prev[i];
			*c->nq[i] = !c->priv->q_prev[i];
		} else {
			*c->q[i]  = true;
			*c->nq[i] = true;
			c->priv->q_prev[i] = rand() % 2;
		}
		c->priv->clk_prev[i] = *c->clk[i];
	}
}
