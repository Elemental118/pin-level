#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#include "a2graphics.h"

struct a2graphics_priv {
	u8 		h;
	u8  		v;
	bool 		phase1_prev;
	unsigned char 	buffer[24][40];
};

u16 graphics_base[24] = {
	0x400,
	0x480,
	0x500,
	0x580,
	0x600,
	0x680,
	0x700,
	0x780,

	0x428,
	0x4A8,
	0x528,
	0x5A8,
	0x628,
	0x6A8,
	0x728,
	0x7A8,

	0x450,
	0x4D0,
	0x550,
	0x5D0,
	0x650,
	0x6D0,
	0x750,
	0x7D0
};

static u8 data(struct a2graphics *c)
{
	u8 data = 0;
	for (int i = 0; i < 8; i++) {
		data |= ((u8)*c->d[i] << i);
	}
	return data;
}

static void set_addr(struct a2graphics *c, u16 addr)
{
	for (int i = 0; i < 16; i++) {
		*c->a[i] = (addr >> i) & 1;
	}
}

static void hv_inc(struct a2graphics *c)
{
	if (++c->priv->h == 40) {
		c->priv->h = 0;
		if (++c->priv->v == 24) {
			c->priv->v = 0;
		}
	}
}

struct a2graphics *a2graphics_create(void)
{
	printf("\033[H\033[2J\033[3J");
	struct a2graphics *c = calloc(1, sizeof(*c));
	c->priv = calloc(1 ,sizeof(*c->priv));
	return c;
}

void a2graphics_free(struct a2graphics *c)
{
	free(c->priv);
	free(c);
}

void a2graphics_tick(struct a2graphics *c)
{
	bool edge = !*c->phase1 && c->priv->phase1_prev;
	c->priv->phase1_prev = *c->phase1;
	if (!edge) {
		return;
	}
	unsigned char ch = data(c) & 0b00111111;
	c->priv->buffer[c->priv->v][c->priv->h] = ch < 0x20 ? ch + 0x40 : ch;
	hv_inc(c);
	set_addr(c, graphics_base[c->priv->v] + c->priv->h);
}

void a2graphics_draw(struct a2graphics *c)
{
	printf("\033[H\033[2J\033[3J");
	for (int v = 0; v < 24; v++) {
		for (int h = 0; h < 40; h++) {
			putchar(c->priv->buffer[v][h]);
		}
		putchar('\n');
	}
}
