#include <stdlib.h>

#include "ttl195.h"

#define CLK_RISING(c) ((bool)(!(c)->priv->clk_prev && *(c)->clk))

struct ttl195_priv {
	bool d_prev[4];
	bool q_prev[4];
	bool load_prev;
	bool clk_prev;
};

struct ttl195 *ttl195_create(void)
{
	struct ttl195 *c = calloc(9, sizeof(*c));
	return c;
}

void ttl195_free(struct ttl195 *c)
{
	free(c);
}

void ttl195_tick(struct ttl195 *c)
{
	if (!*c->clr) {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = false;
		}
	
	} else if (!CLK_RISING(c)) {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = c->priv->q_prev[i];
		}
	
	} else if (!c->priv->load_prev) {
		for (int i = 0; i < 4; i++) {
			*c->q[i] = c->priv->d_prev[i];
		}
	
	} else {
		switch ((u8)*c->j | ((u8)*c->k << 1)) {
		case 0b00:
			*c->q[0] = false;
			break;
		
		case 0b01:
			*c->q[0] = !c->priv->q_prev[0];
			break;
		
		case 0b10:
			*c->q[0] = c->priv->q_prev[0];
			break;
		
		case 0b11:
			*c->q[0] = true;
			break;
		
		default:
			break;
		}
		for (int i = 3; i > 0; i--) {
			*c->q[i] = c->priv->q_prev[i - 1];
		}
	}

	*c->nq = !*c->q[3];

	for (int i = 0; i < 4; i++) {
		c->priv->d_prev[i] = *c->d[i];
		c->priv->q_prev[i] = *c->q[i];
	}
	c->priv->clk_prev = *c->clk;
	c->priv->load_prev = *c->load;
}
