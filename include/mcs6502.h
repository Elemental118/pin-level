#ifndef MCS6502_H
#define MCS6502_H

struct mcs6502 {
	const bool *rdy;
	const bool *phi0;
	const bool *irq;
	const bool *nmi;
	const bool *res;
	bool       *a[16];
	bool       *d[8];
	bool       *rw;

	struct mcs6502_priv *priv;
};

struct mcs6502 *mcs6502_create(bool trace);
void mcs6502_free(struct mcs6502 *c);
void mcs6502_tick(struct mcs6502 *c);

#endif
