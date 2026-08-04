#include <stdlib.h>

#include "ls74.h"

#define CLK_RISING(c, i) ((bool)(!(c)->priv->clk_prev[i] && *(c)->clk[i]))

struct ls74_priv {
	bool q_prev[2];
	bool clk_prev[2];
};

struct ls74 *ls74_create(void)
{
	struct ls74 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));
	return c;
}

void ls74_free(struct ls74 *c)
{
	free(c->priv);
	free(c);
}

void ls74_tick(struct ls74 *c)
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
