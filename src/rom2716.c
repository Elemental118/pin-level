#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rom2716.h"
#include "types.h"

struct rom2716_priv {
	u8 mem[2 * 1024];
};

static u16 addr(struct rom2716 *c)
{
	u16 addr = 0;
	for (int i = 0; i < 11; i++) {
		addr |= ((u16)*c->a[i] << i);
	}

	return addr;
}

struct rom2716 *rom2716_create(int slot)
{
	struct rom2716 *c = calloc(1, sizeof(*c));
	c->priv = calloc(1, sizeof(*c->priv));

	FILE *f;
	switch (slot) {
	case 3:
		f = fopen("roms/e000.bin", "rb");
		break;
	
	case 4:
		f = fopen("roms/e800.bin", "rb");
		break;
	
	case 5:
		f = fopen("roms/f000.bin", "rb");
		break;
	
	case 6:
		f = fopen("roms/f800.bin", "rb");
		break;
		
	default:
		return c;
	}
	u8 *buffer = malloc(0x800 * sizeof(u8));
	if (f == NULL) {
		fprintf(stderr, "could not load ROM in slot %d\n", slot);
		exit(1);
	}
	if (fread(buffer, sizeof(u8), 0x800, f) != 0x800) {
		fprintf(stderr, "could not load ROM in slot %d\n", slot);
		exit(1);
	}
	memcpy(c->priv->mem, buffer, 0x800 * sizeof(u8));
	free(buffer);

	return c;
}

void rom2716_free(struct rom2716 *c)
{
	free(c->priv);
	free(c);
}

void rom2716_tick(struct rom2716 *c)
{
	if (!*c->ce && !*c->oe) {
		u16 a = addr(c);
		for (int i = 0; i < 8; i++) {
			*c->d[i] = (c->priv->mem[a] >> i) & 1;
		}
	}
}
