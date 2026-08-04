#ifndef ROM2513_H
#define ROM2513_H

struct rom2513 {
	const bool *a[9];
	bool       *d[5];
	
	struct rom2513_priv *priv;
};

struct rom2513 *rom2513_create(void);
void rom2513_free(struct rom2513 *c);
void rom2513_tick(struct rom2513 *c);

#endif
