#include <stdlib.h>
#include "types.h"

#include "a2kbd.h"

struct a2kbd_priv {
	unsigned char ascii_code;
	bool ready;
};

struct a2kbd *a2kbd_create(void)
{
	struct a2kbd *c = calloc(1, sizeof(*c));
	c->priv = calloc(1 ,sizeof(*c->priv));
	return c;
}

void a2kbd_free(struct a2kbd *c)
{
	free(c->priv);
	free(c);
}

void a2kbd_tick(struct a2kbd *c)
{
	if (!*c->data_sel) {
		for (int i = 0; i < 8; i++) {
			*c->d[i] = ((c->priv->ascii_code | ((u8)c->priv->ready << 7)) >> i) & 1;
		}
	} else if (!*c->strobe_sel) {
		c->priv->ready = false;
	}
}

void a2kbd_press(struct a2kbd *c, unsigned char character)
{
	c->priv->ascii_code = character;
	c->priv->ready = true;
}
