#ifndef ROM2716_H
#define ROM2716_H

struct rom2716 {
	const bool *a[11];
	bool       *d[8];
	const bool *ce;
	const bool *oe;
	
	struct rom2716_priv *priv;
};

struct rom2716 *rom2716_create(int slot);
void rom2716_free(struct rom2716 *c);
void rom2716_tick(struct rom2716 *c);

#endif
