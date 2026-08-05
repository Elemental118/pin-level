#include <stdlib.h>

#include "ls283.h"

struct ls283 *ls283_create(void)
{
	struct ls283 *c = calloc(1, sizeof(*c));
	return c;
}

void ls283_free(struct ls283 *c)
{
	free(c);
}

void ls283_tick(struct ls283 *c)
{
	u8 a = 0;
	u8 b = 0;
	for (int i = 0; i < 4; i++) {
		a |= ((u8)*c->a[i] << i);
		b |= ((u8)*c->b[i] << i);
	}
	u8 res = a + b + *c->cin;
	for (int i = 0; i < 4; i++) {
		*c->y[i] = (res >> i) & 1;
	}
	*c->cout = (res >> 4) & 1;
}
