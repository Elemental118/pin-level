#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rom2513.h"
#include "types.h"

struct rom2513_priv {
	u8 mem[8 * 64]; // Only bottom 5 bits of every byte are used
};

static u16 addr(struct rom2513 *c)
{
	u16 addr = 0;
	for (int i = 0; i < 11; i++) {
		addr |= ((u16)*c->a[i] << i);
	}

	return addr;
}

struct rom2513 *rom2513_create(void)
{
	struct rom2513 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));

	FILE *f;
	f = fopen("roms/chars.bin", "rb");
	u8 *buffer = malloc(0x200 * sizeof(u8));
	if (f == NULL) {
		fprintf(stderr, "could not load character ROM\n");
		exit(1);
	}
	if (fread(buffer, sizeof(u8), 0x200, f) != 0x200) {
		fprintf(stderr, "could not load character ROM\n");
		exit(1);
	}
	memcpy(c->priv->mem, buffer, 0x200 * sizeof(u8));
	free(buffer);

	return c;
}

void rom2513_free(struct rom2513 *c)
{
	free(c->priv);
	free(c);
}

void rom2513_tick(struct rom2513 *c)
{
	u16 a = addr(c);
	for (int i = 0; i < 5; i++) {
		*c->d[i] = (c->priv->mem[a] >> i) & 1;
	}
}
