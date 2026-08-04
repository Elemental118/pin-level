#include <stdlib.h>
#include <time.h>

#include "mk4116.h"
#include "types.h"

#define RAS_RISING(c)	((bool)(!(c)->priv->prev_ras && *(c)->ras))
#define RAS_FALLING(c)	((bool)((c)->priv->prev_ras && !*(c)->ras))

#define CAS_RISING(c)	((bool)(!(c)->priv->prev_cas && *(c)->cas))
#define CAS_FALLING(c)	((bool)((c)->priv->prev_cas && !*(c)->cas))

struct mk4116_priv {
	bool mem[128 * 128];
	bool prev_ras;
	bool prev_cas;
	u8 latched_row;
};

static u8 addr(struct mk4116 *c)
{
	u8 addr = 0;
	for (int i = 0; i < 7; i++) {
		addr |= ((u8)*c->a[i] << i);
	}

	return addr;
}

struct mk4116 *mk4116_create(void)
{
	struct mk4116 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1 ,sizeof(*c->priv));
	for (int i = 0; i < 128 * 128; i++) {
		c->priv->mem[i] = rand() % 2;
	}
	c->priv->prev_ras = true;
	c->priv->prev_cas = true;
	return c;
}

void mk4116_free(struct mk4116 *c)
{
	free(c->priv);
	free(c);
}

void mk4116_tick(struct mk4116 *c)
{
	u8 a = addr(c);

	if (RAS_FALLING(c)) {
		c->priv->latched_row = a;
	} else if (CAS_FALLING(c) && !*c->we) {
		c->priv->mem[(c->priv->latched_row << 7) | a] = *c->din;
	} else if (CAS_FALLING(c)) {
		*c->dout = c->priv->mem[(c->priv->latched_row << 7) | a];
	}

	c->priv->prev_ras = *c->ras;
	c->priv->prev_cas = *c->cas;
}

