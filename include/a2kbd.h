#ifndef A2KBD_H
#define A2KBD_H

struct a2kbd {
	bool *d[8];
	const bool *data_sel;
	const bool *strobe_sel;
	
	struct a2kbd_priv *priv;
};

struct a2kbd *a2kbd_create(void);
void a2kbd_free(struct a2kbd *c);
void a2kbd_tick(struct a2kbd *c);
void a2kbd_press(struct a2kbd *c, unsigned char character);

#endif
