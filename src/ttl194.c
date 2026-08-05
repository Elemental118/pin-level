#include <stdlib.h>

#include "ttl194.h"

#define CLK_RISING(c) ((bool)(!(c)->priv->clk_prev && *(c)->clk))

struct ttl194_priv {
	bool q_prev[4];
	bool clk_prev;
};

struct ttl194 *ttl194_create(void)
{
	struct ttl194 *c = calloc(9, sizeof(*c));
	return c;
}

void ttl194_free(struct ttl194 *c)
{
	free(c);
}

void ttl194_tick(struct ttl194 *c)
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
		switch ((u8)*c->s[0] | ((u8)*c->s[1] << 1)) {
		case 0b00:
			for (int i = 0; i < 4; i++) {
				*c->q[i] = c->priv->q_prev[i];
			}
			break;
		
		case 0b01:
			for (int i = 3; i > 0; i--) {
				*c->q[i] = *c->q[i - 1];
			}
			*c->q[0] = *c->ser_r;
			break;
		
		case 0b10:
			for (int i = 0; i < 3; i++) {
				*c->q[i] = *c->q[i + 1];
			}
			*c->q[3] = *c->ser_l;
			break;
		
		case 0b11:
			for (int i = 0; i < 4; i++) {
				*c->q[i] = *c->d[i];
			}
			break;
		
		default:
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		c->priv->q_prev[i] = *c->q[i];
	}
	c->priv->clk_prev = *c->clk;
}
