#ifndef A2graphics_H
#define A2graphics_H

struct a2graphics {
	bool       *a[16];
	const bool *d[8];
	const bool *phase1;
	
	struct a2graphics_priv *priv;
};

struct a2graphics *a2graphics_create(void);
void a2graphics_free(struct a2graphics *c);
void a2graphics_tick(struct a2graphics *c);
void a2graphics_draw(struct a2graphics *c);

#endif
