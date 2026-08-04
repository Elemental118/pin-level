#include <stdlib.h>

#include "a2clock.h"

struct a2clock_priv {
	int counter;
};

struct a2clock *a2clock_create(void)
{
	struct a2clock *c = calloc(1, sizeof(*c));
	c->priv = calloc(1 ,sizeof(*c->priv));
	c->priv->counter = -1;
	return c;
}

void a2clock_free(struct a2clock *c)
{
	free(c->priv);
	free(c);
}

void a2clock_tick(struct a2clock *c)
{
	if (++c->priv->counter > 13) {
		c->priv->counter = 0;
	}

	switch (c->priv->counter % 7)
	{
	case 0:
		*c->ras = true;
		*c->ax  = true;
		*c->cas = true;
		break;
	
	case 1:
		*c->ras = false;
		*c->ax  = true;
		*c->cas = true;
		break;
	
	case 2:
		*c->ras = false;
		*c->ax  = false;
		*c->cas = true;
		break;
	
	case 3:
	case 4:
		*c->ras = false;
		*c->ax  = false;
		*c->cas = false;
		break;
	
	case 5:
		*c->ras = false;
		*c->ax  = true;
		*c->cas = false;
		break;
	
	case 6:
		*c->ras = true;
		*c->ax  = true;
		*c->cas = false;
		break;
	
	default:
		break;
	}

	*c->phase0 = (c->priv->counter >= 7);
	*c->phase1 = (c->priv->counter < 7);
}
